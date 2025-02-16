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
#include <tf2/LinearMath/Transform.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2/LinearMath/Vector3.h>

#include <iostream>

#include "kindr_ros/RosTfPose.hpp"
#include "kindr/poses/Pose.hpp"
#include "kindr/rotations/Rotation.hpp"
#include "kindr/common/gtest_eigen.hpp"


TEST(RosTfPoseEigen, convertFromRosTf)
{
  const kindr::RotationQuaternionD referenceQuaternion(0.949, 0.133, 0.169, 0.2305);
  const kindr::RotationMatrixD referenceRotationMatrix(referenceQuaternion);
  const Eigen::Matrix3d referenceRotationMatrixEigen(referenceRotationMatrix.matrix());
  const kindr::Position3D referenceTranslation(0.3, -1.5, 0.6);
  const tf2::Vector3 referenceOriginTf(referenceTranslation.x(),
    referenceTranslation.y(), referenceTranslation.z());
  const tf2::Matrix3x3 referenceBasisTf(referenceRotationMatrixEigen(
      0,
      0), referenceRotationMatrixEigen(
      0, 1), referenceRotationMatrixEigen(0, 2),
    referenceRotationMatrixEigen(1, 0), referenceRotationMatrixEigen(
      1,
      1), referenceRotationMatrixEigen(
      1, 2),
    referenceRotationMatrixEigen(2, 0), referenceRotationMatrixEigen(
      2,
      1), referenceRotationMatrixEigen(
      2, 2));
  const tf2::Quaternion referenceQuaternionTf(referenceQuaternion.x(),
    referenceQuaternion.y(), referenceQuaternion.z(), referenceQuaternion.w());

  kindr::HomogeneousTransformationPosition3RotationQuaternionD pose;
  tf2::Transform tfTransform;
  tf2::Transform tfTransformInverse;

  // Setting & getting identity
  tfTransform.setIdentity();
  kindr_ros::convertFromRosTf(tfTransform, pose);
  kindr::expectNear(
    pose.getTransformationMatrix(),
    Eigen::Matrix4d::Identity(), 1e-4, KINDR_SOURCE_FILE_POS);
  kindr::expectNear(
    pose.getPosition().toImplementation(),
    kindr::Position3D::Zero().toImplementation(), 1e-4, KINDR_SOURCE_FILE_POS);

  // Setting & getting with rotation matrix
  tfTransform.setOrigin(referenceOriginTf);
  tfTransform.setBasis(referenceBasisTf);
  kindr_ros::convertFromRosTf(tfTransform, pose);
  kindr::expectNear(
    kindr::RotationMatrixD(pose.getRotation()).matrix(),
    referenceRotationMatrix.matrix(), 1e-4, KINDR_SOURCE_FILE_POS);
  kindr::expectNear(
    pose.getPosition().toImplementation(),
    referenceTranslation.toImplementation(), 1e-4, KINDR_SOURCE_FILE_POS);

  // Identity transformation
  const kindr::Position3D testVector1(6.6, 0.0, -3.2);
  tfTransform.setIdentity();
  // tf2::Vector3 tfTestVectorTransformed1 =
  //   tfTransform(tf2::Vector3(testVector1.x(), testVector1.y(), testVector1.z()));
  kindr_ros::convertFromRosTf(tfTransform, pose);
  kindr::Position3D testVectorTransformed1 = pose.transform(testVector1);
  EXPECT_NEAR(testVectorTransformed1.x(), testVector1.x(), 1e-8);
  EXPECT_NEAR(testVectorTransformed1.y(), testVector1.y(), 1e-8);
  EXPECT_NEAR(testVectorTransformed1.z(), testVector1.z(), 1e-8);

  // General transformation
  const kindr::Position3D testVector2(-18.4, 41.4, -0.2);
  tfTransform.setOrigin(referenceOriginTf);
  tfTransform.setBasis(referenceBasisTf);
  tf2::Vector3 tfTestVectorTransformed2 =
    tfTransform(tf2::Vector3(testVector2.x(), testVector2.y(), testVector2.z()));
  kindr_ros::convertFromRosTf(tfTransform, pose);
  kindr::Position3D testVectorTransformed2 = pose.transform(testVector2);
  EXPECT_NEAR(testVectorTransformed2.x(), tfTestVectorTransformed2.x(), 1e-2);
  EXPECT_NEAR(testVectorTransformed2.y(), tfTestVectorTransformed2.y(), 1e-2);
  EXPECT_NEAR(testVectorTransformed2.z(), tfTestVectorTransformed2.z(), 1e-2);

  // General inverse transformation
  const kindr::Position3D testVector3(0.6, -0.4, -3.2);
  tfTransform.setOrigin(referenceOriginTf);
  tfTransform.setBasis(referenceBasisTf);
  tfTransformInverse = tfTransform.inverse();
  tf2::Vector3 tfTestVectorTransformed3 =
    tfTransformInverse(tf2::Vector3(testVector3.x(), testVector3.y(), testVector3.z()));
  kindr_ros::convertFromRosTf(tfTransform, pose);
  kindr::Position3D testVectorTransformed3 = pose.inverseTransform(testVector3);
  EXPECT_NEAR(testVectorTransformed3.x(), tfTestVectorTransformed3.x(), 1e-2);
  EXPECT_NEAR(testVectorTransformed3.y(), tfTestVectorTransformed3.y(), 1e-2);
  EXPECT_NEAR(testVectorTransformed3.z(), tfTestVectorTransformed3.z(), 1e-2);

  // Zero vector transformation
  const kindr::Position3D testVector4(0.0, 0.0, 0.0);
  tfTransform.setOrigin(referenceOriginTf);
  tfTransform.setBasis(referenceBasisTf);
  tf2::Vector3 tfTestVectorTransformed4 =
    tfTransform(tf2::Vector3(testVector4.x(), testVector4.y(), testVector4.z()));
  kindr_ros::convertFromRosTf(tfTransform, pose);
  kindr::Position3D testVectorTransformed4 = pose.transform(testVector4);
  EXPECT_NEAR(testVectorTransformed4.x(), tfTestVectorTransformed4.x(), 1e-4);
  EXPECT_NEAR(testVectorTransformed4.y(), tfTestVectorTransformed4.y(), 1e-4);
  EXPECT_NEAR(testVectorTransformed4.z(), tfTestVectorTransformed4.z(), 1e-4);

  // Only rotation transformation
  const kindr::Position3D testVector5(1.0, 3.0, -4.0);
  tfTransform.setIdentity();
  tfTransform.setBasis(referenceBasisTf);
  tf2::Vector3 tfTestVectorTransformed5 =
    tfTransform(tf2::Vector3(testVector5.x(), testVector5.y(), testVector5.z()));
  kindr_ros::convertFromRosTf(tfTransform, pose);
  kindr::Position3D testVectorTransformed5 = pose.transform(testVector5);
  EXPECT_NEAR(testVectorTransformed5.x(), tfTestVectorTransformed5.x(), 1e-2);
  EXPECT_NEAR(testVectorTransformed5.y(), tfTestVectorTransformed5.y(), 1e-2);
  EXPECT_NEAR(testVectorTransformed5.z(), tfTestVectorTransformed5.z(), 1e-2);

  // Only translation
  const kindr::Position3D testVector6(0.0, 1.0, -44.0);
  tfTransform.setIdentity();
  tfTransform.setOrigin(referenceOriginTf);
  tf2::Vector3 tfTestVectorTransformed6 =
    tfTransform(tf2::Vector3(testVector6.x(), testVector6.y(), testVector6.z()));
  kindr_ros::convertFromRosTf(tfTransform, pose);
  kindr::Position3D testVectorTransformed6 = pose.transform(testVector6);
  EXPECT_NEAR(testVectorTransformed6.x(), tfTestVectorTransformed6.x(), 1e-8);
  EXPECT_NEAR(testVectorTransformed6.y(), tfTestVectorTransformed6.y(), 1e-8);
  EXPECT_NEAR(testVectorTransformed6.z(), tfTestVectorTransformed6.z(), 1e-8);
}

TEST(RosTfPoseEigen, convertToRosTf)
{
  const kindr::RotationQuaternionD referenceQuaternion(0.113, 0.071, -0.924, 0.3585);
  const kindr::RotationMatrixD referenceRotationMatrix(referenceQuaternion);
  const kindr::Position3D referenceTranslation(-22.4, 0.31, -4.6);

  kindr::HomogeneousTransformationPosition3RotationQuaternionD pose;
  tf2::Transform tfTransform;
  tf2::Transform tfTransformInverse;

  // Identity transformation
  const kindr::Position3D testVector1(0.6, -0.4, -3.2);
  pose.setIdentity();
  // kindr::Position3D testVector1Transformed = pose.transform(testVector1);
  kindr_ros::convertToRosTf(pose, tfTransform);
  tf2::Vector3 tfTestVector1Transformed =
    tfTransform(tf2::Vector3(testVector1.x(), testVector1.y(), testVector1.z()));
  EXPECT_NEAR(tfTestVector1Transformed.x(), testVector1.x(), 1e-8);
  EXPECT_NEAR(tfTestVector1Transformed.y(), testVector1.y(), 1e-8);
  EXPECT_NEAR(tfTestVector1Transformed.z(), testVector1.z(), 1e-8);

  // General transformation
  const kindr::Position3D testVector2(57.6, -4.4, 333.2);
  pose.getPosition() = referenceTranslation;
  pose.getRotation() = referenceQuaternion;
  kindr::Position3D testVector2Transformed = pose.transform(testVector2);
  kindr_ros::convertToRosTf(pose, tfTransform);
  tf2::Vector3 tfTestVector2Transformed =
    tfTransform(tf2::Vector3(testVector2.x(), testVector2.y(), testVector2.z()));
  EXPECT_NEAR(tfTestVector2Transformed.x(), testVector2Transformed.x(), 1e-8);
  EXPECT_NEAR(tfTestVector2Transformed.y(), testVector2Transformed.y(), 1e-8);
  EXPECT_NEAR(tfTestVector2Transformed.z(), testVector2Transformed.z(), 1e-8);

  // General inverse transformation
  const kindr::Position3D testVector3(27.6, 41.4, -3.2);
  pose.getPosition() = referenceTranslation;
  pose.getRotation() = referenceQuaternion;
  kindr::Position3D testVector3Transformed = pose.inverseTransform(testVector3);
  kindr_ros::convertToRosTf(pose, tfTransform);
  tfTransformInverse = tfTransform.inverse();
  tf2::Vector3 tfTestVector3Transformed =
    tfTransformInverse(tf2::Vector3(testVector3.x(), testVector3.y(), testVector3.z()));
  EXPECT_NEAR(tfTestVector3Transformed.x(), testVector3Transformed.x(), 1e-8);
  EXPECT_NEAR(tfTestVector3Transformed.y(), testVector3Transformed.y(), 1e-8);
  EXPECT_NEAR(tfTestVector3Transformed.z(), testVector3Transformed.z(), 1e-8);

  // Zero vector transformation
  const kindr::Position3D testVector4(0.0, 0.0, 0.0);
  pose.getPosition() = referenceTranslation;
  pose.getRotation() = referenceQuaternion;
  kindr::Position3D testVector4Transformed = pose.transform(testVector4);
  kindr_ros::convertToRosTf(pose, tfTransform);
  tf2::Vector3 tfTestVector4Transformed =
    tfTransform(tf2::Vector3(testVector4.x(), testVector4.y(), testVector4.z()));
  EXPECT_NEAR(tfTestVector4Transformed.x(), testVector4Transformed.x(), 1e-8);
  EXPECT_NEAR(tfTestVector4Transformed.y(), testVector4Transformed.y(), 1e-8);
  EXPECT_NEAR(tfTestVector4Transformed.z(), testVector4Transformed.z(), 1e-8);

  // Only rotation transformation
  const kindr::Position3D testVector5(1.0, 3.0, -4.0);
  pose.setIdentity();
  pose.getRotation() = referenceQuaternion;
  kindr::Position3D testVector5Transformed = pose.transform(testVector5);
  kindr_ros::convertToRosTf(pose, tfTransform);
  tf2::Vector3 tfTestVector5Transformed =
    tfTransform(tf2::Vector3(testVector5.x(), testVector5.y(), testVector5.z()));
  EXPECT_NEAR(tfTestVector5Transformed.x(), testVector5Transformed.x(), 1e-8);
  EXPECT_NEAR(tfTestVector5Transformed.y(), testVector5Transformed.y(), 1e-8);
  EXPECT_NEAR(tfTestVector5Transformed.z(), testVector5Transformed.z(), 1e-8);

  // Only translation
  const kindr::Position3D testVector6(0.0, 1.0, -44.0);
  pose.setIdentity();
  pose.getPosition() = referenceTranslation;
  kindr::Position3D testVector6Transformed = pose.transform(testVector6);
  kindr_ros::convertToRosTf(pose, tfTransform);
  tf2::Vector3 tfTestVector6Transformed =
    tfTransform(tf2::Vector3(testVector6.x(), testVector6.y(), testVector6.z()));
  EXPECT_NEAR(tfTestVector6Transformed.x(), testVector6Transformed.x(), 1e-8);
  EXPECT_NEAR(tfTestVector6Transformed.y(), testVector6Transformed.y(), 1e-8);
  EXPECT_NEAR(tfTestVector6Transformed.z(), testVector6Transformed.z(), 1e-8);
}

TEST(RosTfPoseEigen, convertTwoWays)
{
  const kindr::RotationQuaternionD referenceQuaternion(0.212, 0.0421, -0.958, 0.1885);
  const kindr::RotationMatrixD referenceRotationMatrix(referenceQuaternion);
  const Eigen::Matrix3d referenceRotationMatrixEigen(referenceRotationMatrix.matrix());
  const kindr::Position3D referenceTranslation(13.3, 2.6, -7.6);
  const tf2::Vector3 referenceOriginTf(referenceTranslation.x(),
    referenceTranslation.y(), referenceTranslation.z());
  const tf2::Matrix3x3 referenceBasisTf(referenceRotationMatrixEigen(
      0,
      0), referenceRotationMatrixEigen(
      0, 1), referenceRotationMatrixEigen(0, 2),
    referenceRotationMatrixEigen(1, 0), referenceRotationMatrixEigen(
      1,
      1), referenceRotationMatrixEigen(
      1, 2),
    referenceRotationMatrixEigen(2, 0), referenceRotationMatrixEigen(
      2,
      1), referenceRotationMatrixEigen(
      2, 2));
  const tf2::Quaternion referenceQuaternionTf(referenceQuaternion.x(),
    referenceQuaternion.y(), referenceQuaternion.z(), referenceQuaternion.w());

  kindr::HomogeneousTransformationPosition3RotationQuaternionD pose, poseConverted;
  tf2::Transform tfTransform, tfTransformConverted;

  // Kindr -> TF -> Kindr
  pose.getPosition() = referenceTranslation;
  pose.getRotation() = referenceQuaternion;
  kindr_ros::convertToRosTf(pose, tfTransform);
  kindr_ros::convertFromRosTf(tfTransform, poseConverted);
  kindr::expectNear(
    poseConverted.getTransformationMatrix(),
    pose.getTransformationMatrix(), 1e-4, KINDR_SOURCE_FILE_POS);

  // TF -> Kindr -> TF
  tfTransform.setOrigin(referenceOriginTf);
  tfTransform.setBasis(referenceBasisTf);
  kindr_ros::convertFromRosTf(tfTransform, pose);
  kindr_ros::convertToRosTf(pose, tfTransformConverted);
  EXPECT_NEAR(
    tfTransformConverted.getBasis().getRow(0).x(),
    tfTransform.getBasis().getRow(0).x(), 1e-8);
  EXPECT_NEAR(
    tfTransformConverted.getBasis().getRow(0).y(),
    tfTransform.getBasis().getRow(0).y(), 1e-8);
  EXPECT_NEAR(
    tfTransformConverted.getBasis().getRow(0).z(),
    tfTransform.getBasis().getRow(0).z(), 1e-8);
  EXPECT_NEAR(
    tfTransformConverted.getBasis().getRow(1).x(),
    tfTransform.getBasis().getRow(1).x(), 1e-8);
  EXPECT_NEAR(
    tfTransformConverted.getBasis().getRow(1).y(),
    tfTransform.getBasis().getRow(1).y(), 1e-8);
  EXPECT_NEAR(
    tfTransformConverted.getBasis().getRow(1).z(),
    tfTransform.getBasis().getRow(1).z(), 1e-8);
  EXPECT_NEAR(
    tfTransformConverted.getBasis().getRow(2).x(),
    tfTransform.getBasis().getRow(2).x(), 1e-8);
  EXPECT_NEAR(
    tfTransformConverted.getBasis().getRow(2).y(),
    tfTransform.getBasis().getRow(2).y(), 1e-8);
  EXPECT_NEAR(
    tfTransformConverted.getBasis().getRow(2).z(),
    tfTransform.getBasis().getRow(2).z(), 1e-8);

  EXPECT_NEAR(tfTransformConverted.getOrigin().x(), tfTransform.getOrigin().x(), 1e-8);
  EXPECT_NEAR(tfTransformConverted.getOrigin().y(), tfTransform.getOrigin().y(), 1e-8);
  EXPECT_NEAR(tfTransformConverted.getOrigin().z(), tfTransform.getOrigin().z(), 1e-8);
}
