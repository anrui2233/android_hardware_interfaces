/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_HARDWARE_INTERFACES_NEURALNETWORKS_1_3_UTILS_PREPARED_MODEL_H
#define ANDROID_HARDWARE_INTERFACES_NEURALNETWORKS_1_3_UTILS_PREPARED_MODEL_H

#include <android/hardware/neuralnetworks/1.3/IPreparedModel.h>
#include <nnapi/IPreparedModel.h>
#include <nnapi/Result.h>
#include <nnapi/Types.h>
#include <nnapi/hal/1.0/ProtectCallback.h>
#include <nnapi/hal/CommonUtils.h>

#include <memory>
#include <tuple>
#include <utility>
#include <vector>

// See hardware/interfaces/neuralnetworks/utils/README.md for more information on HIDL interface
// lifetimes across processes and for protecting asynchronous calls across HIDL.

namespace android::hardware::neuralnetworks::V1_3::utils {

// Class that adapts V1_3::IPreparedModel to nn::IPreparedModel.
class PreparedModel final : public nn::IPreparedModel,
                            public std::enable_shared_from_this<PreparedModel> {
    struct PrivateConstructorTag {};

  public:
    static nn::GeneralResult<std::shared_ptr<const PreparedModel>> create(
            sp<V1_3::IPreparedModel> preparedModel, bool executeSynchronously);

    PreparedModel(PrivateConstructorTag tag, bool executeSynchronously,
                  sp<V1_3::IPreparedModel> preparedModel, hal::utils::DeathHandler deathHandler);

    nn::ExecutionResult<std::pair<std::vector<nn::OutputShape>, nn::Timing>> execute(
            const nn::Request& request, nn::MeasureTiming measure,
            const nn::OptionalTimePoint& deadline, const nn::OptionalDuration& loopTimeoutDuration,
            const std::vector<nn::TokenValuePair>& hints,
            const std::vector<nn::ExtensionNameAndPrefix>& extensionNameToPrefix) const override;

    nn::GeneralResult<std::pair<nn::SyncFence, nn::ExecuteFencedInfoCallback>> executeFenced(
            const nn::Request& request, const std::vector<nn::SyncFence>& waitFor,
            nn::MeasureTiming measure, const nn::OptionalTimePoint& deadline,
            const nn::OptionalDuration& loopTimeoutDuration,
            const nn::OptionalDuration& timeoutDurationAfterFence,
            const std::vector<nn::TokenValuePair>& hints,
            const std::vector<nn::ExtensionNameAndPrefix>& extensionNameToPrefix) const override;

    nn::GeneralResult<nn::SharedExecution> createReusableExecution(
            const nn::Request& request, nn::MeasureTiming measure,
            const nn::OptionalDuration& loopTimeoutDuration,
            const std::vector<nn::TokenValuePair>& hints,
            const std::vector<nn::ExtensionNameAndPrefix>& extensionNameToPrefix) const override;

    nn::GeneralResult<nn::SharedBurst> configureExecutionBurst() const override;

    std::any getUnderlyingResource() const override;

    nn::ExecutionResult<std::pair<std::vector<nn::OutputShape>, nn::Timing>> executeInternal(
            const Request& request, V1_2::MeasureTiming measure, const OptionalTimePoint& deadline,
            const OptionalTimeoutDuration& loopTimeoutDuration,
            const hal::utils::RequestRelocation& relocation) const;

    nn::GeneralResult<std::pair<nn::SyncFence, nn::ExecuteFencedInfoCallback>>
    executeFencedInternal(const Request& request, const hidl_vec<hidl_handle>& waitFor,
                          V1_2::MeasureTiming measure, const OptionalTimePoint& deadline,
                          const OptionalTimeoutDuration& loopTimeoutDuration,
                          const OptionalTimeoutDuration& timeoutDurationAfterFence,
                          const hal::utils::RequestRelocation& relocation) const;

  private:
    nn::ExecutionResult<std::pair<std::vector<nn::OutputShape>, nn::Timing>> executeSynchronously(
            const Request& request, V1_2::MeasureTiming measure, const OptionalTimePoint& deadline,
            const OptionalTimeoutDuration& loopTimeoutDuration) const;
    nn::ExecutionResult<std::pair<std::vector<nn::OutputShape>, nn::Timing>> executeAsynchronously(
            const Request& request, V1_2::MeasureTiming measure, const OptionalTimePoint& deadline,
            const OptionalTimeoutDuration& loopTimeoutDuration) const;

    const bool kExecuteSynchronously;
    const sp<V1_3::IPreparedModel> kPreparedModel;
    const hal::utils::DeathHandler kDeathHandler;
};

}  // namespace android::hardware::neuralnetworks::V1_3::utils

#endif  // ANDROID_HARDWARE_INTERFACES_NEURALNETWORKS_1_3_UTILS_PREPARED_MODEL_H
