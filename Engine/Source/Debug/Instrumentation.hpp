#pragma once

#include "QMBTPCH.hpp"

#include "Core/Aliases.hpp"
#include "Core/Logging/Logger.hpp"
#include "Core/Macros.hpp"
#include "Core/Types/Array.hpp"
#include "Core/Types/String.hpp"
#include "Core/Types/Vector.hpp"

namespace QMBT
{

	enum class ProfileCategory : UInt8
	{
		Core,
		Application,
		Layers,
		Rendering,
		Window,
		Physics,
		Editor,
		Other
	};

	enum class TimeUnit : UInt8
	{
		NanoSeconds,
		MicroSeconds,
		MilliSeconds,
	};

	using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;
	using FloatingPointMilliseconds = std::chrono::duration<double, std::milli>;
	using FloatingPointNanoseconds = std::chrono::duration<double, std::nano>;

	struct ProfileData
	{
		const char* Name;
		double StartTime;
		double EndTime;
		double ElapsedTime;
		std::thread::id ThreadID;
		ProfileCategory Category;

		// Size: 48, Alignment: 8
	};

	struct Frame
	{
		Vector<Vector<ProfileData>> Data;
	};

	struct InstrumentationSession
	{
		String Name;
	};

	using TimesArray = Array<Vector<double>, static_cast<int>(ProfileCategory::Other) + 1>;

	// Singleton class
	class Instrumentor
	{
	  public:
		Instrumentor(const Instrumentor&) = delete;
		Instrumentor(Instrumentor&&) = delete;

		void BeginSession()
		{
			std::lock_guard<std::mutex> lock(m_Mutex);

			if (m_CurrentSession)
			{
				// If there is already a current session, then close it before beginning new one.
				// Subsequent profiling output meant for the original session will end up in the
				// newly opened session instead.  That's better than having badly formatted
				// profiling output.
				if (Logger::GetCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
				{
					LOG_CORE_ERROR("Instrumentor::BeginSession() when session one already open.");
				}
				InternalEndSession();
			}

			m_CurrentSession = new InstrumentationSession();
			for (auto& vec : m_Times)
			{
				vec.clear();
			}

			m_Frames.clear();

			m_FirstFrame = true;
			m_Paused = false;
			m_SessionStartTime = GetCurrentTime();
		}

		void EndSession()
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			InternalEndSession();
		}

		void AddProfile(ProfileData data)
		{
			data.StartTime -= m_SessionStartTime; // Normalize the time, making it the time from the start of the session
			data.EndTime -= m_SessionStartTime;

			bool found = false;
			Size rowIndex = 0;

			while (!found)
			{
				if (rowIndex < m_CurrentFrame.Data.size())
				{
					auto& row = m_CurrentFrame.Data[rowIndex];
					if (data.StartTime > row.back().EndTime)
					{
						row.push_back(data);
						found = true;
					}
				}
				else
				{
					m_CurrentFrame.Data.push_back(Vector<ProfileData>());
					m_CurrentFrame.Data.back().push_back(data);
					found = true;
				}

				rowIndex++;
			}

			// for (auto& func : m_AddCallbacks)
			// {
			// 	func(dataPtr.get());
			// }
		}

		void BeginFrame()
		{
			m_CurrentFrameStartTime = GetCurrentTime();
		}

		void EndFrame()
		{
			// Update the m_Times array with data from the current frame;
			if (IsRecording())
			{
				for (auto& vec : m_Times)
				{
					vec.push_back(0);
				}
				Array<double, static_cast<int>(ProfileCategory::Other) + 1> endTimes{};

				for (auto& row : m_CurrentFrame.Data)
				{
					for (auto& data : row) // Loop through all the data in all the rows
					{
						int categoryIndex = static_cast<int>(data.Category);
						// If the current data doesn't belong to a nested function
						if (data.StartTime > endTimes[categoryIndex])
						{
							// Increment the total time
							m_Times[categoryIndex].back() += data.ElapsedTime;
							endTimes[categoryIndex] = data.EndTime;
						}
					}
				}

				m_Frames.push_back(m_CurrentFrame);
				m_CurrentFrame.Data.clear();

				m_TotalFrameTimes.push_back(GetCurrentTime() - m_CurrentFrameStartTime);
			}

			if (m_FirstFrame)
			{
				m_Recording = true;
				m_FirstFrame = false;
			}
		}

		inline bool IsRecording() const
		{
			return m_Recording && !m_Paused;
		}
		inline bool IsStopped() const
		{
			return !m_Recording;
		}
		inline bool IsPaused() const
		{
			return m_Paused;
		}

		void Pause() { m_Paused = true; }

		void Resume() { m_Paused = false; }

		inline double GetCurrentTime() const
		{
			auto time = std::chrono::steady_clock::now().time_since_epoch();
			switch (m_TimeUnit)
			{
			case TimeUnit::MilliSeconds:
				return FloatingPointMilliseconds{time}.count();
				break;
			case TimeUnit::MicroSeconds:
				return FloatingPointMicroseconds{time}.count();
				break;
			case TimeUnit::NanoSeconds:
				return FloatingPointNanoseconds{time}.count();
				break;
			}
			LOG_CORE_ERROR("Unknown time unit!");
			return 0;
		}

		Vector<Frame>* GetFrames()
		{
			return &m_Frames;
		}

		TimesArray* GetCategoryTimes() { return &m_Times; }
		double* GetTotalTimes() { return &m_TotalFrameTimes[0]; }
		double GetFrameTime(int index) const { return m_TotalFrameTimes[index]; }

		static Instrumentor&
		GetInstance()
		{
			static Instrumentor instance;
			return instance;
		}

	  private:
		Instrumentor()
			: m_CurrentSession(nullptr),
			  m_Frames("Profiler Frames"),
			  m_TotalFrameTimes("Total Frame Times Allocator")
		{
			m_TotalFrameTimes.reserve(10000);
			m_Frames.reserve(10000);
			for (auto& vec : m_Times)
			{
				vec.reserve(10000);
			}
		}

		~Instrumentor()
		{
			EndSession();
		}

		// Note: you must already own lock on m_Mutex before
		// calling InternalEndSession()
		void InternalEndSession()
		{
			if (m_CurrentSession)
			{
				delete m_CurrentSession;
				m_CurrentSession = nullptr;
				m_Recording = false;
			}
		}

	  private:
		std::mutex m_Mutex;
		InstrumentationSession* m_CurrentSession;

		TimesArray m_Times;
		Vector<double> m_TotalFrameTimes;

		Vector<Frame> m_Frames;
		Frame m_CurrentFrame;
		double m_CurrentFrameStartTime;

		double m_SessionStartTime;
		bool m_Recording;
		bool m_FirstFrame = false;
		bool m_Paused;

		TimeUnit m_TimeUnit = TimeUnit::MilliSeconds;
	};

	class InstrumentationTimer
	{
	  public:
		explicit InstrumentationTimer(const char* name, ProfileCategory category = ProfileCategory::Other)
			: m_Name(name), m_Category(category), m_Started(false)
		{
			if (Instrumentor::GetInstance().IsRecording())
			{
				m_StartTime = Instrumentor::GetInstance().GetCurrentTime();
				m_Started = true;
			}
		}

		~InstrumentationTimer()
		{
			if (m_Started)
			{
				double endTime = Instrumentor::GetInstance().GetCurrentTime();
				Instrumentor::GetInstance()
					.AddProfile({m_Name,
								 m_StartTime,
								 endTime,
								 endTime - m_StartTime,
								 std::this_thread::get_id(),
								 m_Category});
			}
		}

	  private:
		const char* m_Name;
		double m_StartTime;
		bool m_Started;
		ProfileCategory m_Category;
	};
} // namespace QMBT

#if QMBT_DEBUG

#define PROFILE_SCOPE_LINE_INTERNAL2(name, line) \
	::QMBT::InstrumentationTimer timer##line(name);

#define PROFILE_SCOPE_LINE_INTERNAL3(name, line, group) \
	::QMBT::InstrumentationTimer timer##line(name, group);

#define PROFILE_SCOPE_LINE_INTERNAL(...)                                                 \
	GET_MACRO_3(__VA_ARGS__, PROFILE_SCOPE_LINE_INTERNAL3, PROFILE_SCOPE_LINE_INTERNAL2) \
	(__VA_ARGS__)

#define PROFILE_SCOPE_LINE2(name, line) PROFILE_SCOPE_LINE_INTERNAL(name, line)
#define PROFILE_SCOPE_LINE3(name, line, group) PROFILE_SCOPE_LINE_INTERNAL(name, line, group)
#define PROFILE_SCOPE_LINE(...)                                        \
	GET_MACRO_3(__VA_ARGS__, PROFILE_SCOPE_LINE3, PROFILE_SCOPE_LINE2) \
	(__VA_ARGS__)

#define PROFILE_SCOPE2(name) PROFILE_SCOPE_LINE(name, __LINE__)
#define PROFILE_SCOPE3(name, group) PROFILE_SCOPE_LINE(name, __LINE__, group)
#define PROFILE_SCOPE(...)                                   \
	GET_MACRO_2(__VA_ARGS__, PROFILE_SCOPE3, PROFILE_SCOPE2) \
	(__VA_ARGS__)

#define PROFILE_FUNCTION2() PROFILE_SCOPE(QMBT_FUNC_SIG)
#define PROFILE_FUNCTION3(group) PROFILE_SCOPE(QMBT_FUNC_SIG, group)
#define PROFILE_FUNCTION(...)                                      \
	GET_MACRO_1(__VA_ARGS__, PROFILE_FUNCTION3, PROFILE_FUNCTION2) \
	(__VA_ARGS__)
#else
#define PROFILE_SCOPE(...)
#define PROFILE_FUNCTION(...)

#endif
