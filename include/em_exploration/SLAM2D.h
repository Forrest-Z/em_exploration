#ifndef EM_EXPLORATION_SLAM2D_H
#define EM_EXPLORATION_SLAM2D_H

#include <fstream>
#include <gtsam/geometry/Point2.h>
#include <gtsam/geometry/Pose2.h>
#include <gtsam/sam/BearingRangeFactor.h>
#include <gtsam/slam/BetweenFactor.h>
#include <gtsam/slam/PriorFactor.h>
#include <gtsam/nonlinear/ISAM2.h>
#include <gtsam/nonlinear/Marginals.h>

#include "em_exploration/Simulation2D.h"

namespace em_exploration {

class SLAM2D {

 public:
  typedef gtsam::BearingRangeFactor<Pose2, Point2> MeasurementFactor2D;
  typedef boost::shared_ptr<MeasurementFactor2D> MeasurementFactor2DPtr;
  typedef gtsam::BetweenFactor<Pose2> OdometryFactor2D;
  typedef boost::shared_ptr<gtsam::BetweenFactor<gtsam::Pose2>> OdometryFactor2DPtr;

  SLAM2D(const Map::Parameter &parameter);

  ~SLAM2D() {}

  void printParameters() const;

  void fromISAM2(std::shared_ptr<gtsam::ISAM2> isam, const Map &map, const gtsam::Values values);

  void addPrior(unsigned int key, const LandmarkBeliefState &landmark_state);

  void addPrior(const VehicleBeliefState &vehicle_state);

  static OdometryFactor2DPtr buildOdometryFactor(unsigned int x1, unsigned int x2,
                                                 const SimpleControlModel::ControlState &control_state);

  void addOdometry(const SimpleControlModel::ControlState &control_state);

  static MeasurementFactor2DPtr buildMeasurementFactor(unsigned int x, unsigned int l,
                                                       const BearingRangeSensorModel::Measurement &measurement);

  void addMeasurement(unsigned int key, const BearingRangeSensorModel::Measurement &measurement);

  /// Save graph to .dot file
  /// Visualize the graph with `dot -Tpdf ./graph.dot -O`"
  void saveGraph(const std::string &name = std::string("graph.dot")) const;

  void printGraph() const;

  Eigen::MatrixXd jointMarginalCovarianceLocal(const std::vector<unsigned int> &poses,
                                          const std::vector<unsigned int> &landmarks) const;

  Eigen::MatrixXd jointMarginalCovariance(const std::vector<unsigned int> &poses,
                                  const std::vector<unsigned int> &landmarks) const;

  std::shared_ptr<const gtsam::ISAM2> getISAM2() const;

  /// Perform optimization and update the map including best estimate.
  /// The covariance of the lastest pose is updated if update_covariacne is true.
  void optimize(bool update_covariance = true);

  const Map &getMap() const { return map_; }

  static gtsam::Symbol getVehicleSymbol(unsigned int key) {
    return gtsam::Symbol('x', key);
  }

  static gtsam::Symbol getLandmarkSymbol(unsigned int key) {
    return gtsam::Symbol('l', key);
  }

 private:

  Map map_;
  unsigned int step_;

  std::shared_ptr<gtsam::ISAM2> isam_;

  mutable bool marginals_update_;
  mutable std::shared_ptr<gtsam::Marginals> marginals_;

  gtsam::NonlinearFactorGraph graph_;
  gtsam::Values initial_estimate_;
  gtsam::Values result_;
  bool optimized_;
};
}
#endif //EM_EXPLORATION_SLAM2D_H
