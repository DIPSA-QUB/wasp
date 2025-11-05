#include "benchmark.h"
#include "command_line.h"
#include "util.h"
#include "writer.h"

using namespace std;

int main(int argc, char* argv[]) {
  CLExport cli(argc, argv, "weights");
  cli.parse();

  auto suffix = GetSuffix(cli.graph_filename());
  if (suffix != ".wsg") {
    std::cout << "Graph format must be: .wsg" << std::endl;
    std::exit(-1);
  }

  WeightedBuilder b(cli);
  WGraph g = b.MakeGraph();
  g.PrintStats();

  WeightsWriter ww(g);
  ww.WriteWeightsSerialized(cli.out_filename());

  VectorReader<WeightT> reader(cli.out_filename());
  auto weights = reader.ReadSerialized("Weights");

  auto index = g.out_index();
  std::size_t wi = 0;
  for (std::size_t i = 0; i < g.num_nodes(); i++) {
    auto edge_list = index[i];
    std::size_t degree = index[i + 1] - index[i];
    for (std::size_t j = 0; j < degree; j++) {
      if (edge_list[j].w != weights[wi++]) {
        std::cerr << "Weights mismatch for edge (" << i << "," << edge_list[j].v << ")" << std::endl;
        std::cerr << edge_list[j].w << "!=" << weights[wi++] << std::endl;
      }
    }
  }
  std::cout << "Sanity check passed" << std::endl;
}