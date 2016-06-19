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

#include "../dependencies/Catch/single_include/catch.hpp"
#include "../dependencies/json/src/json.hpp"
#include "../dependencies/gorynych/gorynych/gorynych.h"
#include "../dependencies/gorynych/gorynych-test/test_extensions.h"

#include "../solowej/noise/noisegenerators.h"
#include "../solowej/engine/compiler.h"


namespace zzsystems { namespace solowej { namespace  test {
    using namespace gorynych;
    using namespace gorynych::test;
    using namespace solowej::engine;

#define TYPE_PREFIX TEST_PREFIX("compiler")

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

            compiler<capability> cc(false);

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

            compiler<capability> cc(false);

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

            compiler<capability> cc(false);

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

            compiler<capability> cc(false);

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

            compiler<capability> cc(false);

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

            compiler<capability> cc(false);

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

            compiler<capability> cc(false);

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

            compiler<capability> cc(false);

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

            compiler<capability> cc(false);

            REQUIRE_THROWS_AS(cc.compile(source), cyclic_module_ref_error);
        }
}}}
