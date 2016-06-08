//
// Created by szuyev on 19.05.16.
//

#include "../Catch/single_include/catch.hpp"
#include "../json/src/json.hpp"
#include "../gorynych/gorynych/gorynych.h"
#include "../gorynych/gorynych/unit_test_ext.h"
#include "../solowej/noise/noisegenerators.h"
#include "../solowej/engine/compiler.h"

#include <sstream>


namespace zzsystems { namespace solowej { namespace test {
            using namespace gorynych;
            using namespace gorynych::test;


#if defined(COMPILE_AVX2)
            using capability = capability_AVX2;
#define TYPE_PREFIX "compiler - avx2"
#elif defined(COMPILE_AVX1)
            using capability = capability_AVX1;
#define TYPE_PREFIX "compiler - avx1"
#elif defined(COMPILE_SSE4FMA)
            using capability = capability_SSE4FMA;
    #define TYPE_PREFIX "compiler - sse4 fma"
#elif defined(COMPILE_SSE4)
            using capability = capability_SSE4;
#define TYPE_PREFIX "compiler - sse4"
#elif defined(COMPILE_SSSE3)
            using capability = capability_SSSE3;
    #define TYPE_PREFIX "compiler - ssse3"
#elif defined(COMPILE_SSE3)
            using capability = capability_SSE3;
    #define TYPE_PREFIX "compiler - sse3"
#elif defined(COMPILE_SSE2)
            using capability = capability_SSE2;
    #define TYPE_PREFIX "compiler - sse2"
#else
            using vtest = capability_FPU;
#define TYPE_PREFIX "compiler - x87"
#endif

            using vreal = static_dispatcher<capability>::vreal;
            using vint = static_dispatcher<capability>::vint;

            using sint = int;
            using sreal = float;


        TEST_CASE(TYPE_PREFIX"compile simple", "[compiler]")
        {
            nlohmann::json source =
            {
                { "version", "0.9" },
                { "environment",
                    {
                        { "max_feature_set", -1 },
                        { "entryPoint", "output"},
                        { "scheduler",
                          {
                              {"max_feature_set", -1},
                              {"dimensions", {512, 512, 1}},
                              {"use_threads", true}
                          }
                        }
                    }
                },
                { "modules",
                    {
                        {
                            { "name", "output" },
                            { "type", "generator" },
                            { "module", "perlin" },
                            { "source", {}},
                            { "settings",
                                {
                                    { "frequency", 8.0 },
                                    { "lacunarity", 2.18359375 },
                                    { "persistence", 0.625 },
                                    { "octaves", 6 },
                                    { "seed", 100 }
                                }
                            }
                        }
                    }
                }
            };

            INFO("Compiling: " << source.dump(4));

            compiler<SIMD_T> cc(false);

            REQUIRE_NOTHROW(cc.compile(source));
        }

        TEST_CASE(TYPE_PREFIX"check transformer build", "[compiler]")
        {

        }

        TEST_CASE(TYPE_PREFIX"invalid enty point", "[compiler]")
        {
            nlohmann::json source =
            {
                {"version", "0.9"},
                { "environment",
                  {
                    { "max_feature_set", -1 },
                    { "entryPoint", "output_invalid"},
                    { "scheduler",
                      {
                          { "dimensions", {512, 512, 1}},
                          { "use_threads", true}
                      }
                    }
                  }
                },
                { "modules",
                    {
                        {
                            { "name",   "output"},
                            { "type",   "generator"},
                            { "module", "perlin"},
                            { "source", {}},
                            { "settings",
                                {
                                    {"frequency", 8.0,},
                                    {"lacunarity", 2.18359375,},
                                    {"persistence", 0.625,},
                                    {"octaves", 6,},
                                    {"seed", 100,}
                                }
                            }
                        }
                    }
                }
            };

            INFO("Compiling: " << source.dump(4));

            compiler<SIMD_T> cc(false);

            REQUIRE_THROWS_AS(cc.compile(source), unresolved_node_reference_error);
        }

        TEST_CASE(TYPE_PREFIX"missing enty point", "[compiler]")
        {
            nlohmann::json source =
            {
                { "version", "0.9" },
                { "environment",
                  {
                    { "max_feature_set", -1 },
                    { "entryPoint", ""},
                    { "scheduler",
                      {
                          { "dimensions", { 512, 512, 1}},
                          { "use_threads", true}
                      }
                    }
                  }
                },
                { "modules",
                    {
                        {
                            {"name", "output" },
                            { "type", "generator" },
                            { "module", "perlin" },
                            { "source", {}},
                            { "settings",
                                {
                                    { "frequency", 8.0 },
                                    { "lacunarity", 2.18359375 },
                                    { "persistence", 0.625 },
                                    { "octaves", 6 },
                                    { "seed", 100 }
                                }
                            }
                        }
                    }
                }
            };

            INFO("Compiling: " << source.dump(4));

            compiler<SIMD_T> cc(false);

            REQUIRE_THROWS_AS(cc.compile(source), invalid_entry_point_error);
        }

        TEST_CASE(TYPE_PREFIX"invalid scheduler data", "[compiler]")
        {
            nlohmann::json source =
            {
                { "version", "0.9" },
                { "environment",
                  {
                    { "max_feature_set", -1 },
                    { "entryPoint", "output"},
                    { "scheduler",
                      {
                          { "dimensions", { -1, -1, 1}},
                          { "use_threads", true}
                      }
                    }
                  }
                },
                { "modules",
                    {
                        {
                            { "name", "output" },
                            { "type", "generator" },
                            { "module", "perlin" },
                            { "source", {}},
                            { "settings",
                                {
                                    { "frequency", 8.0 },
                                    { "lacunarity", 2.18359375 },
                                    { "persistence", 0.625 },
                                    { "octaves", 6 },
                                    { "seed", 100 }
                                }
                            }
                        }
                    }
                }
            };

            compiler<SIMD_T> cc(false);

            // TODO
            //REQUIRE_THROWS_AS(cc.compile(source), invalid_entry_point_error);
        }

        TEST_CASE(TYPE_PREFIX"duplicates", "[compiler]")
        {
            nlohmann::json source =
            {
                { "version", "0.9" },
                { "environment",
                  {
                    { "max_feature_set", -1 },
                    { "entryPoint", "output"},
                    { "scheduler",
                      {
                          { "dimensions", { 512, 512, 1}},
                          { "use_threads", true}
                      }
                    }
                  }
                },
                { "modules",
                    {
                        {
                            { "name", "output" },
                            { "type", "generator" },
                            { "module", "perlin" },
                            { "source", {}},
                            { "settings",
                                {
                                    { "frequency", 8.0 },
                                    { "lacunarity", 2.18359375 },
                                    { "persistence", 0.625 },
                                    { "octaves", 6 },
                                    { "seed", 100 }
                                }
                            }
                        },
                        {
                            { "name", "output" },
                            { "type", "generator" },
                            { "module", "billow" },
                            { "source", {}},
                            { "settings",
                                {
                                    { "frequency", 8.0 },
                                    { "lacunarity", 2.18359375 },
                                    { "persistence", 0.625 },
                                    { "octaves", 6 },
                                    { "seed", 100 }
                                }
                            }
                        }
                    }
                }
            };

            INFO("Compiling: " << source.dump(4));

            compiler<SIMD_T> cc(false);

            REQUIRE_THROWS_AS(cc.compile(source), duplicate_module_error);
        }

        TEST_CASE(TYPE_PREFIX"unresolved reference", "[compiler]")
        {
            nlohmann::json source =
            {
                { "version", "0.9" },
                { "environment",
                  {
                    { "max_feature_set", -1 },
                    { "entryPoint", "output"},
                    { "scheduler",
                      {
                          { "dimensions", { 512, 512, 1}},
                          { "use_threads", true}
                      }
                    }
                  }
                },
                { "modules",
                    {
                        {
                            { "name", "output" },
                            { "type", "generator_invalid" },
                            { "module", "perlin" },
                            { "source", {}},
                            { "settings",
                                {
                                    { "frequency", 8.0 },
                                    { "lacunarity", 2.18359375 },
                                    { "persistence", 0.625 },
                                    { "octaves", 6 },
                                    { "seed", 100 }
                                }
                            }
                        }
                    }
                }
            };


            INFO("Compiling: " << source.dump(4));

            compiler<SIMD_T> cc(false);

            REQUIRE_THROWS_AS(cc.compile(source), unresolved_module_reference_error);
        }



        TEST_CASE(TYPE_PREFIX"missing sources", "[compiler]")
        {
            nlohmann::json source =
            {
                { "version", "0.9" },
                { "environment",
                  {
                    { "max_feature_set", -1 },
                    { "entryPoint", "output"},
                    { "scheduler",
                      {
                          { "dimensions", { 512, 512, 1}},
                          { "use_threads", true}
                      }
                    }
                  }
                },
                { "modules",
                    {
                        {
                            { "name", "generator" },
                            { "type", "generator" },
                            { "module", "perlin" },
                            { "source", {}},
                            { "settings",
                                {
                                    { "frequency", 8.0 },
                                    { "lacunarity", 2.18359375 },
                                    { "persistence", 0.625 },
                                    { "octaves", 6 },
                                    { "seed", 100 }
                                }
                            }
                        },
                        {
                            { "name", "output" },
                            { "type", "modifier" },
                            { "module", "add" },
                            { "source", { "generator_invalid" }},
                            { "settings",
                                {
                                    { "frequency", 8.0 },
                                    { "lacunarity", 2.18359375 },
                                    { "persistence", 0.625 },
                                    { "octaves", 6 },
                                    { "seed", 100 }
                                }
                            }
                        }
                    }
                }
            };

            INFO("Compiling: " << source.dump(4));

            compiler<SIMD_T> cc(false);

            REQUIRE_THROWS_AS(cc.compile(source), invalid_module_error);
        }

        TEST_CASE(TYPE_PREFIX"wrong module data", "[compiler]")
        {
            nlohmann::json source =
            {
                { "version", "0.9" },
                { "environment",
                  {
                    { "max_feature_set", -1 },
                    { "entryPoint", "output"},
                    { "scheduler",
                      {
                          { "dimensions", { 512, 512, 1}},
                          { "use_threads", true}
                      }
                    }
                  }
                },
                { "modules",
                    {
                        {
                            { "name", "generator" },
                            { "type", "generator" },
                            { "module", "perlin" },
                            { "source", {}},
                            { "settings",
                                {
                                    { "frequency", 8.0 },
                                    { "lacunarity", 2.18359375 },
                                    { "persistence", 0.625 },
                                    { "octaves", 6 },
                                    { "seed", 100 }
                                }
                            }
                        },
                        {
                            { "name", "output" },
                            { "type", "modifier" },
                            { "module", "curve" },
                            { "source", { "generator" }},
                            { "settings",
                                { "points",
                                    {}
                                }
                            }
                        }
                    }
                }
            };

            INFO("Compiling: " << source.dump(4));

            compiler<SIMD_T> cc(false);

            REQUIRE_THROWS_AS(cc.compile(source), invalid_module_error);
        }

        TEST_CASE(TYPE_PREFIX"cycles", "[compiler]")
        {
            nlohmann::json source =
            {
                { "version", "0.9" },
                { "environment",
                  {
                    { "max_feature_set", -1 },
                    { "entryPoint", "output_cyclic_ref"},
                    { "scheduler",
                      {
                          { "dimensions", { 512, 512, 1}},
                          { "use_threads", true}
                      }
                    }
                  }
                },
                { "modules",
                    {
                        {
                            { "name", "generator" },
                            { "type", "generator" },
                            { "module", "perlin" },
                            { "source", {}},
                            { "settings",
                                {
                                    { "frequency", 8.0 },
                                    { "lacunarity", 2.18359375 },
                                    { "persistence", 0.625 },
                                    { "octaves", 6 },
                                    { "seed", 100 }
                                }
                            }
                        },
                        {
                            { "name", "output_cyclic_ref" },
                            { "type", "modifier" },
                            { "module", "add" },
                            { "source", { "generator", "output_cyclic_ref" }}
                        }
                    }
                }
            };

            INFO("Compiling: " << source.dump(4));

            compiler<SIMD_T> cc(false);

            REQUIRE_THROWS_AS(cc.compile(source), cyclic_module_ref_error);
        }
}}}
