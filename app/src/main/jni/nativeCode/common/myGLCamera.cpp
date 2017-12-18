/*
 *    Copyright 2016 Anand Muralidhar
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */


#include "myGLCamera.h"
#include "math.h"

MyGLCamera::MyGLCamera(
        float FOV,
        float zPosition,
        float nearPlaneDistance,
        float farPlaneDistance) {

    // camera position is fixed
    glm::vec3 cameraPosition = glm::vec3(0, 0, zPosition);
    viewMat = glm::lookAt(cameraPosition,        // Camera location in World Space
                          glm::vec3(0, 0, -1),   // direction in which camera it is pointed
                          glm::vec3(0, 1, 0));   // camera is pointing up

    this->nearPlaneDistance = nearPlaneDistance;
    this->farPlaneDistance = farPlaneDistance;
    this->FOV = FOV;

    // 6DOF describing model's position
    deltaX = deltaY = deltaZ = 0;                  // translations
    modelQuaternion = glm::quat(glm::vec3(0, 0, 0)); // rotation

    modelMat = glm::mat4(1.0f);
    translateMat = glm::mat4(1.0f);
    rotateMat = glm::mat4(1.0f);
    mvpMat = glm::mat4(1.0f); // projection is not known -> initialize MVP to identity
    this->cameraPosition = glm::vec3(M_PI / 2, 0, 100);
}

/**
 * Use the display's aspect ratio to compute projection matrix
 */
void MyGLCamera::SetAspectRatio(float aspect) {

    glm::mat4 projectionMat;
    projectionMat = glm::perspective(FOV * float(M_PI / 180), // camera's field-of-view
                                     aspect,                  // camera's aspect ratio
                                     nearPlaneDistance,       // distance to the near plane
                                     farPlaneDistance);       // distance to the far plane
    projectionViewMat = projectionMat * viewMat;
    ComputeMVPMatrix();

}

/**
 * Model's position has 6 degrees-of-freedom: 3 for x-y-z locations and
 * 3 for alpha-beta-gamma Euler angles
 * Convert euler angles to quaternion and update MVP
 */
void MyGLCamera::SetModelPosition(std::vector<float> modelPosition) {

    deltaX = modelPosition[0];
    deltaY = modelPosition[1];
    deltaZ = modelPosition[2];
    float pitchAngle = modelPosition[3];
    float yawAngle = modelPosition[4];
    float rollAngle = modelPosition[5];

    modelQuaternion = glm::quat(glm::vec3(pitchAngle, yawAngle, rollAngle));
    rotateMat = glm::toMat4(modelQuaternion);
    ComputeMVPMatrix();
}


/**
 * Compute the translation matrix from x-y-z position and rotation matrix from
 * quaternion describing the rotation
 * MVP = Projection * View * (Translation * Rotation)
 */
void MyGLCamera::ComputeMVPMatrix() {

    translateMat = glm::mat4(1, 0, 0, 0,                  // col0
                             0, 1, 0, 0,                  // col1
                             0, 0, 1, 0,                  // col2
                             deltaX, deltaY, deltaZ, 1);  // col3

    modelMat = translateMat * rotateMat;
    mvpMat = projectionViewMat * modelMat;
}

/**
 * Simulate change in scale by pushing or pulling the model along Z axis
 */
void MyGLCamera::ScaleModel(float scaleFactor) {
    this->cameraPosition.z -= SCALE_TO_Z_TRANSLATION * (scaleFactor - 1);
    ComputeMVPMatrix();
}

/**
 * Finger drag movements are converted to rotation of model by deriving a
 * quaternion from the drag movement
 */
void MyGLCamera::RotateModel(float distanceX, float distanceY) {
    cameraPosition.x = cameraPosition.x - distanceX / 1000;
    if (cameraPosition.x > 2 * M_PI) {
        cameraPosition.x = cameraPosition.x - 2 * M_PI;
    }
    cameraPosition.y = cameraPosition.y - distanceY / 1000;
    if (cameraPosition.y >= M_PI / 2) {
        cameraPosition.y = M_PI / 2 - 0.001;
    }
    if (cameraPosition.y < 0) {
        cameraPosition.y = 0;
    }
    cameraPosition = glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);
    ComputeMVPMatrix();
}

/**
 * displace model by changing x-y coordinates
 */
void MyGLCamera::TranslateModel(float distanceX, float distanceY) {
    deltaX += XY_TRANSLATION_FACTOR * distanceX;
    deltaY += XY_TRANSLATION_FACTOR * distanceY;
    ComputeMVPMatrix();
}