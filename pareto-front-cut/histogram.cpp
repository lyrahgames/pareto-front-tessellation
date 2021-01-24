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
  if ((argc < 3) || (argc > 4)) {
    cout << "Usage:\n"
         << argv[0]
         << " <pareto input data file> <output file> [<bin_count>]\n";
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
  real max_distance = 0;
  real min_distance = INFINITY;
  real mean_distance = 0;
  for (size_t i = 0; i < pareto_front.size() - 1; ++i) {
    const auto [x1, y1] = pareto_front[i];
    const auto [x2, y2] = pareto_front[i + 1];
    const auto distance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    mean_distance += distance;
    max_distance = max(max_distance, distance);
    min_distance = min(min_distance, distance);
  }
  mean_distance /= pareto_front.size() - 1;

  cout << setw(30) << "mean distance = " << mean_distance << '\n'
       << setw(30) << "max distance = " << max_distance << '\n'
       << setw(30) << "min distance = " << min_distance << '\n';

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

  size_t bin_count = 100;
  if (argc == 4) stringstream{argv[3]} >> bin_count;
  cout << setw(30) << "bin count = " << bin_count << '\n';
  // local helper functions
  // const auto distance_to_bin = [&](real distance) {
  //   constexpr real factor = bin_count / log(2);
  //   return static_cast<size_t>(floor(factor * log(real{1} + distance)));
  // };
  // const auto bin_to_distance = [&](size_t bin) {
  //   constexpr real factor = log(2) / bin_count;
  //   return (exp(factor * bin) - real{1});
  // };

  vector<real> histogram(bin_count);
  // vector<real> histogram(1 + distance_to_bin(max_distance));
  for (size_t i = 0; i < pareto_front.size() - 1; ++i) {
    const auto [x1, y1] = pareto_front[i];
    const auto [x2, y2] = pareto_front[i + 1];
    const auto distance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    const size_t index =
        distance / std::nextafter(max_distance, INFINITY) * bin_count;
    ++histogram[index];
    // ++histogram[distance_to_bin(distance)];
  }
  for (auto& x : histogram) x /= pareto_front.size() - 1;

  fstream output{argv[2], ios::out};
  for (size_t i = 0; i < histogram.size(); ++i) {
    const auto box_position =
        real{0.5} *
        (i * std::nextafter(max_distance, INFINITY) / bin_count +
         (i + 1) * std::nextafter(max_distance, INFINITY) / bin_count);
    const auto box_width =
        (i + 1) * std::nextafter(max_distance, INFINITY) / bin_count -
        i * std::nextafter(max_distance, INFINITY) / bin_count;
    output << setw(20) << box_position << setw(20) << histogram[i] << setw(20)
           << box_width << '\n';
    // const auto box_position =
    //     real{0.5} * (bin_to_distance(i) + bin_to_distance(i + 1));
    // const auto box_width = bin_to_distance(i + 1) - bin_to_distance(i);
    // output << setw(20) << box_position << setw(20) << histogram[i] <<
    // setw(20)
    //        << box_width << '\n';
  }
  output << flush;

  gnuplot_pipe plot{};
  plot << "set logscale y\n"
       << "plot '" << argv[2] << "' w boxes lc black fs solid 0.2\n";
}