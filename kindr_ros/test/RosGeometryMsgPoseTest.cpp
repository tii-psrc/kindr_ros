/*
 * Copyright (c) 2014, Peter Fankhauser, Christian Gehring, Hannes Sommer, Paul Furgale, Remo Diethelm
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Autonomous Systems Lab, ETH Zurich nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Peter Fankhauser, Christian Gehring, Hannes Sommer, Paul Furgale,
 * Remo Diethelm BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include <Eigen/Core>
#include <gtest/gtest.h>

// ROS
#include <geometry_msgs/msg/pose.hpp>

#include <iostream>

#include "kindr_ros/RosGeometryMsgPose.hpp"
#include "kindr/poses/Pose.hpp"
#include "kindr/common/gtest_eigen.hpp"


TEST(RosGeometryMsgPoseEigen, convertFromRosGeometryMsg)
{
  const kindr::Position3D referenceTranslation(0.3, -1.5, 0.6);
  const kindr::RotationQuaternionPD referenceQuaternion(0.113, 0.071, -0.924, 0.35835);

  geometry_msgs::msg::Pose geometryPoseMsg;
  geometryPoseMsg.orientation.x = referenceQuaternion.x();
  geometryPoseMsg.orientation.y = referenceQuaternion.y();
  geometryPoseMsg.orientation.z = referenceQuaternion.z();
  geometryPoseMsg.orientation.w = referenceQuaternion.w();
  geometryPoseMsg.position.x = referenceTranslation.x();
  geometryPoseMsg.position.y = referenceTranslation.y();
  geometryPoseMsg.position.z = referenceTranslation.z();

  kindr::HomogeneousTransformationPosition3RotationQuaternionD pose;
  kindr_ros::convertFromRosGeometryMsg(geometryPoseMsg, pose);

  kindr::expectNear(
    pose.getPosition().toImplementation(), referenceTranslation.toImplementation(), 1e-4,
    KINDR_SOURCE_FILE_POS);
  EXPECT_TRUE(pose.getRotation().isNear(referenceQuaternion, 1e-8));
}

TEST(RosGeometryMsgPoseEigen, convertToRosGeometryMsg)
{
  const kindr::Position3D referenceTranslation(0.3, -1.1, -0.6);
  const kindr::RotationQuaternionPD referenceQuaternion(0.212, 0.0421, -0.958, 0.1885);

  kindr::HomogeneousTransformationPosition3RotationQuaternionD pose;
  pose.getPosition() = referenceTranslation;
  pose.getRotation() = referenceQuaternion;

  geometry_msgs::msg::Pose geometryPoseMsg;
  kindr_ros::convertToRosGeometryMsg(pose, geometryPoseMsg);

  EXPECT_NEAR(geometryPoseMsg.orientation.x, referenceQuaternion.x(), 1e-8);
  EXPECT_NEAR(geometryPoseMsg.orientation.y, referenceQuaternion.y(), 1e-8);
  EXPECT_NEAR(geometryPoseMsg.orientation.z, referenceQuaternion.z(), 1e-8);
  EXPECT_NEAR(geometryPoseMsg.orientation.w, referenceQuaternion.w(), 1e-8);
  EXPECT_NEAR(geometryPoseMsg.position.x, referenceTranslation.x(), 1e-8);
  EXPECT_NEAR(geometryPoseMsg.position.y, referenceTranslation.y(), 1e-8);
  EXPECT_NEAR(geometryPoseMsg.position.z, referenceTranslation.z(), 1e-8);
}

TEST(RosGeometryMsgTransformationEigen, convertFromRosGeometryMsg)
{
  const kindr::Position3D referenceTranslation(0.12, 1.5, 0.6);
  const kindr::RotationQuaternionPD referenceQuaternion(0.949, 0.133, 0.169, 0.230541);

  geometry_msgs::msg::Transform geometryTransformMsg;
  geometryTransformMsg.rotation.x = referenceQuaternion.x();
  geometryTransformMsg.rotation.y = referenceQuaternion.y();
  geometryTransformMsg.rotation.z = referenceQuaternion.z();
  geometryTransformMsg.rotation.w = referenceQuaternion.w();
  geometryTransformMsg.translation.x = referenceTranslation.x();
  geometryTransformMsg.translation.y = referenceTranslation.y();
  geometryTransformMsg.translation.z = referenceTranslation.z();

  kindr::HomogeneousTransformationPosition3RotationQuaternionD transformation;
  kindr_ros::convertFromRosGeometryMsg(geometryTransformMsg, transformation);

  kindr::expectNear(
    transformation.getPosition().toImplementation(), referenceTranslation.toImplementation(), 1e-4,
    KINDR_SOURCE_FILE_POS);
  EXPECT_TRUE(transformation.getRotation().isNear(referenceQuaternion, 1e-8));
}

TEST(RosGeometryMsgTransformationEigen, convertToRosGeometryMsg)
{
  const kindr::Position3D referenceTranslation(0.3, -1.1345, -0.6);
  const kindr::RotationQuaternionPD referenceQuaternion(0.212, 0.0421, -0.958, 0.1885);

  kindr::HomogeneousTransformationPosition3RotationQuaternionD transformation;
  transformation.getPosition() = referenceTranslation;
  transformation.getRotation() = referenceQuaternion;

  geometry_msgs::msg::Transform geometryTransformMsg;
  kindr_ros::convertToRosGeometryMsg(transformation, geometryTransformMsg);

  EXPECT_NEAR(geometryTransformMsg.rotation.x, referenceQuaternion.x(), 1e-8);
  EXPECT_NEAR(geometryTransformMsg.rotation.y, referenceQuaternion.y(), 1e-8);
  EXPECT_NEAR(geometryTransformMsg.rotation.z, referenceQuaternion.z(), 1e-8);
  EXPECT_NEAR(geometryTransformMsg.rotation.w, referenceQuaternion.w(), 1e-8);
  EXPECT_NEAR(geometryTransformMsg.translation.x, referenceTranslation.x(), 1e-8);
  EXPECT_NEAR(geometryTransformMsg.translation.y, referenceTranslation.y(), 1e-8);
  EXPECT_NEAR(geometryTransformMsg.translation.z, referenceTranslation.z(), 1e-8);
}
