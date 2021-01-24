#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
//
#include <lyrahgames/gnuplot_pipe.hpp>

using namespace std;
using namespace lyrahgames;

using real = float;
struct point {
  real x, y;
};

int main(int argc, char** argv) {
  if (3 != argc) {
    cout << "Usage:\n"
         << argv[0] << " <pareto input data file> <output file>\n";
    return -1;
  }

  fstream file{argv[1], ios::in};
  vector<point> pareto_front{};
  string line;
  while (getline(file, line)) {
    stringstream s{line};
    point p;
    s >> p.x >> p.y;
    pareto_front.push_back(p);
  }

  cout << setw(30) << "size = " << pareto_front.size() << '\n';

  // Compute mean value.
  real mean_distance = 0;
  for (size_t i = 0; i < pareto_front.size() - 1; ++i) {
    const auto [x1, y1] = pareto_front[i];
    const auto [x2, y2] = pareto_front[i + 1];
    mean_distance += sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
  }
  mean_distance /= pareto_front.size() - 1;

  cout << setw(30) << "mean distance = " << mean_distance << '\n';

  // Compute the variance and standard deviation.
  real var_distance = 0;
  for (size_t i = 0; i < pareto_front.size() - 1; ++i) {
    const auto [x1, y1] = pareto_front[i];
    const auto [x2, y2] = pareto_front[i + 1];
    const auto distance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    var_distance += (distance - mean_distance) * (distance - mean_distance);
  }
  var_distance /= pareto_front.size() - 2;
  auto stddev_distance = sqrt(var_distance);

  cout << setw(30) << "distance variance = " << var_distance << '\n'
       << setw(30) << "distance stddev = " << stddev_distance << '\n';

  fstream output{argv[2], ios::out};
  for (size_t i = 0; i < pareto_front.size() - 1; ++i) {
    const auto [x1, y1] = pareto_front[i];
    const auto [x2, y2] = pareto_front[i + 1];
    const auto distance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    output << setw(20) << x1 << setw(20) << y1 << '\n';
    if (distance > mean_distance + 2 * stddev_distance) {
      cout << "Cut connection (" << i << ", " << i + 1 << ").\n";
      output << '\n';
    }
  }
  output << setw(20) << pareto_front.back().x << setw(20)
         << pareto_front.back().y << '\n'
         << flush;

  gnuplot_pipe plot{};
  plot << "plot '" << argv[2] << "' u 1:2 w lp\n";
}