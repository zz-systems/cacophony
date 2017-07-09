//
// Vectorized and parallelized version of libnoise using the gorynych SIMD toolkit
// Copyright (C) 2015-2016 Sergej Zuyev (sergej.zuyev - at - zz-systems.net)
//
// Original libnoise:
// Copyright (C) 2003, 2004 Jason Bevins
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig'.)
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "json.hpp"
#include "zacc.hpp"

#include "gtest/gtest.h"
#include "util/testing/gtest_ext.hpp"
#include "noise/noisegenerators.hpp"
#include "platform/compilers/compiler.hpp"


namespace cacophony { namespace test {
    using namespace zacc;
    using namespace zacc::test;
    using namespace cacophony::platform;

    using vector_branch = dispatched_branch::types;

        TEST(compiler, compile_simple)
        {
            REQUIRES(ZACC_CAPABILITIES);

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

            //SCOPED_TRACE("Compiling: ") << source.dump(4);

            compiler<vector_branch> cc(false);

            ASSERT_NO_THROW(cc.compile(source));
        }

        TEST(compiler, transformer_build)
        {

        }

        TEST(compiler, invalid_entry_point)
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

            //INFO("Compiling: " << source.dump(4));

            compiler<vector_branch> cc(false);

            ASSERT_THROW(cc.compile(source), unresolved_node_reference_error);
        }

        TEST(compiler, missing_entry_point)
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

            //INFO("Compiling: " << source.dump(4));

            compiler<vector_branch> cc(false);

            ASSERT_THROW(cc.compile(source), invalid_entry_point_error);
        }

        TEST(compiler, invalid_scheduler_data)
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

            compiler<vector_branch> cc(false);

            // TODO
            //ASSERT_THROW(cc.compile(source), invalid_entry_point_error);
        }

        TEST(compiler, duplicates)
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

            //INFO("Compiling: " << source.dump(4));

            compiler<vector_branch> cc(false);

            ASSERT_THROW(cc.compile(source), duplicate_module_error);
        }

        TEST(compiler, unresolved_reference)
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


            //INFO("Compiling: " << source.dump(4));

            compiler<vector_branch> cc(false);

            ASSERT_THROW(cc.compile(source), unresolved_module_reference_error);
        }



        TEST(compiler, missing_sources)
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

            //INFO("Compiling: " << source.dump(4));

            compiler<vector_branch> cc(false);

            ASSERT_THROW(cc.compile(source), invalid_module_error);
        }

        TEST(compiler, wrong_module_data)
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

            //INFO("Compiling: " << source.dump(4));

            compiler<vector_branch> cc(false);

            ASSERT_THROW(cc.compile(source), invalid_module_error);
        }

        TEST(compiler, cycles)
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

            //INFO("Compiling: " << source.dump(4));

            compiler<vector_branch> cc(false);

            ASSERT_THROW(cc.compile(source), cyclic_module_ref_error);
        }
}}
