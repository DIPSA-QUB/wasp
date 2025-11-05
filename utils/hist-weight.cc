#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <iostream>
#include <limits>
#include <string>

#include "benchmark.h"
#include "command_line.h"

int main(int argc, char* argv[]) {

  CLExport cli(argc, argv, "weights histogram");
  cli.parse();

  std::vector<WeightT> weights;
  WeightedBuilder b(cli);

  {
    WGraph g = b.MakeGraph();
    std::cout << "Creating weights list" << std::endl;
    weights.reserve(g.num_edges_directed());

    for (auto i = 0; i < g.num_nodes(); i++) {
      for (WNode wn : g.out_neigh(i)) {
        weights.push_back(wn.w);
      }
    }
  }
  std::cout << "Sorting weights list" << std::endl;
  std::sort(std::execution::par, weights.begin(), weights.end());

  std::cout << "Min weight: " << weights[0] << std::endl;
  std::cout << "Max weight: " << weights[weights.size() - 1] << std::endl;

  auto num_weights = weights.size();
  std::vector<WeightT> values;
  std::vector<uint64_t> frequencies;

  uint64_t count = 0;
  for (size_t i = 0; i < num_weights; ++i) {
    const WeightT value = weights[i];

    count++;

    if ((i == num_weights - 1) || (weights[i] != weights[i + 1])) {
      values.push_back(value);
      frequencies.push_back(count);
      count = 0;
    }
  }

  std::cout << "Number of distinct weights: " << values.size() << std::endl;

  std::ofstream weights_file;
  weights_file.open(cli.out_filename());
  weights_file << "weight," << "frequency\n";
  weights_file << std::setprecision(std::numeric_limits<WeightT>::max_digits10);
  for (std::size_t i = 0; i < values.size(); i++) {
    weights_file << values[i] << "," << frequencies[i] << "\n";
  }
  return 0;
}
