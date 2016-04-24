#pragma once

#include "dependencies.h"
#include "parser.h"
#include "scheduler.h"
#include <fstream>
namespace zzsystems { namespace paranoise
{
	using namespace simdal;
	using namespace scheduler;

	class engine :
		public serializable<nlohmann::json>
	{		
	public:
		system_info info;
		string file_name;
		scheduler_settings settings;

		engine() = default;

		const nlohmann::json &operator<<(const nlohmann::json &source) override
		{
			auto environment = source["environment"];
			int feature_set = environment.value("max_feature_set", 0xFFFF'FFFF);

			info.feature_flags &= feature_set;	

			compile(source);

			return source;
		}
		
		void compile_file(const string &path)
		{
			ifstream file(path);
			json source(file);
			
			*this << source;
		}

		void compile_imm_str(const string &content)
		{
			json source = json::parse(content);

			*this << source;
		}

		void compile(const nlohmann::json &source)
		{
			settings << source["environment"]["scheduler"];

			SIMD_DISPATCH_ALL(info,
			{
				cout << ">>building " << static_dispatcher<capability>::unit_name() << " branch" << endl;

				auto& scheduler = (scheduler_cache<vreal, vint>());
				scheduler.source = (compile_module<vreal, vint>(source));
				scheduler.transform = [](const auto&c) {return c;};
				scheduler << source["environment"]["scheduler"];
			});
		}

		float* run(const vec3<float> &origin)
		{
			SIMD_DISPATCH(info,
			{
				return (exec<capability, vreal, vint>(origin));
			});
		}

		void run(float *target, const vec3<float> &origin)
		{
			SIMD_DISPATCH(info,
			{
				return (exec<capability, vreal, vint>(target, origin));
			});
		}
	private:
		SIMD_ENABLED
		auto compile_module(const nlohmann::json &source)
		{
			parser<SIMD_T> p;
			return p.parse(source);
		}

		SIMD_ENABLED
		cpu_scheduler<vreal> &scheduler_cache()
		{
			static cpu_scheduler<vreal> scheduler;

			return scheduler;
		}


		template<typename capability, typename vreal, typename vint>
		auto exec(const vec3<float> &origin)
		{
			cout << ">>dispatch: using " << static_dispatcher<capability>::unit_name() << " branch" << endl;
			
			return scheduler_cache<vreal, vint>()(origin);
		}

		template<typename capability, typename vreal, typename vint>
		void exec(float *target, const vec3<float> &origin)
		{
			cout << ">>dispatch: using " << static_dispatcher<capability>::unit_name() << " branch" << endl;

			scheduler_cache<vreal, vint>()(target, origin);
		}
	};
}}