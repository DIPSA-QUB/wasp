#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

#include "benchmark.h"
#include "command_line.h"

int main(int argc, char* argv[]) {
  CLExport cli(argc, argv, "degree histogram");
  cli.parse();

  WeightedBuilder b(cli);
  WGraph g = b.MakeGraph();

  std::vector<std::atomic<uint64_t>> degree_hist(g.num_nodes());

  std::cout << "Building degree histogram" << std::endl;

#pragma omp parallel for
  for (auto i = 0; i < g.num_nodes(); i++) {
    degree_hist[g.out_degree(i)].fetch_add(1);
  }

  std::cout << "Outputting" << std::endl;

  std::ofstream weights_file;
  weights_file.open(cli.out_filename());
  weights_file << "degree," << "frequency\n";
  for (std::size_t i = 0; i < degree_hist.size(); i++) {
    if (degree_hist[i] > 0)
      weights_file << i << "," << degree_hist[i] << "\n";
  }

  return 0;
}
