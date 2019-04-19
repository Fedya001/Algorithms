#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>
#include <iomanip>
#include <cassert>
#include <fstream>

template<class T>
void PrintVector(const std::vector<T>& vec) {
  for (const auto& elem : vec) {
    std::cout << elem << ' ';
  }
  std::cout << std::endl;
}

struct Point {
  int x;
  int y;
  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }
};

std::istream& operator>>(std::istream& in_stream, Point& p) {
  return in_stream >> p.x >> p.y;
}

std::ostream& operator<<(std::ostream& out_stream, const Point& p) {
  return out_stream << '(' << p.x << "; " << p.y << ')';
}

Point DownLeftPoint(const std::vector<Point>& points) {
  assert(!points.empty());
  Point best_point = points[0];
  for (const auto& point : points) {
    if (std::tie(point.y, point.x) < std::tie(best_point.y, best_point.x)) {
      best_point = point;
    }
  }
  return best_point;
}

bool IsLeftDirection(const Point& base, const Point& first,
                     const Point& second) {
  int k_ort_value = (second.y - base.y) * (first.x - base.x)
      - (second.x - base.x) * (first.y - base.y);
  return k_ort_value > 0;
}

std::vector<Point> ConvexHull(std::vector<Point> points) {
  if (points.size() <= 2) {
    return points;
  }

  Point base = DownLeftPoint(points);
  points.erase(std::find(points.begin(), points.end(), base));

  sort(points.begin(), points.end(),
       [&base](const auto& lhs, const auto& rhs) {
         return IsLeftDirection(base, lhs, rhs);
       });

  std::vector<Point> point_stack;
  point_stack.insert(point_stack.end(), {base, points[0], points[1]});
  for (unsigned i = 2; i < points.size(); ++i) {
    while (!IsLeftDirection(*(point_stack.end() - 2),
                            point_stack.back(), points[i])) {
      point_stack.pop_back();
    }
    point_stack.push_back(points[i]);
  }

  return point_stack;
}

long long ComputeSegmentArea(const Point& first, const Point& second,
                               const Point& base) {
  return 1ll * (first.x - second.x) *
      (2ll * (std::min(first.y, second.y) - base.y)
          + abs(first.y - second.y));
}

double PolygonArea(const std::vector<Point>& polygon) {
  if (polygon.size() <= 2) {
    return 0.0;
  }
  Point base = DownLeftPoint(polygon);

  long long volume = 0;
  for (unsigned i = 1; i < polygon.size(); ++i) {
    volume += ComputeSegmentArea(polygon[i - 1], polygon[i], base);
  }
  volume += ComputeSegmentArea(polygon.back(), polygon.front(), base);

  return volume / 2.0;
}

int main() {
  std::ifstream input("polygon.txt");

  unsigned num_points;
  input >> num_points;

  std::vector<Point> points(num_points);
  for (auto& point : points) {
    input >> point;
  }

  auto convex_hull = ConvexHull(points);
  std::cout << "ConvexHull : ";
  PrintVector(convex_hull);

  std::cout << "Volume = " << PolygonArea(convex_hull);

  return 0;
}
