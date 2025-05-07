#!/bin/sh
rm -rf lib
rm -rf include

bazel build -c opt --incompatible_use_platforms_repo_for_constraints --experimental_cc_shared_library lyra:lyra_shared

## copy libs
mkdir lib
cp bazel-bin/lyra/liblyra_shared.so lib

## copy headers
external_dir=/home/toor/.cache/bazel/_bazel_toor/32d1308fa4bb0d1c73b88c1541ac9d67/external
mkdir -p include/lyra
cp lyra/*.h include/lyra
cp bazel-bin/lyra/lyra_config.pb.h include/lyra
mkdir -p include/absl
cp -r $external_dir/com_google_absl/absl include
mkdir -p include/ghc
cp -r $external_dir/gulrak_filesystem/include include
mkdir -p include/glog
cp -r bazel-lyra/external/com_google_glog/src/glog/*.h include/glog
cp -r bazel-bin/external/com_google_glog/src/glog/*.h include/glog
mkdir -p include/gflags
cp -r bazel-bin/external/com_github_gflags_gflags/*.h include/gflags

## copy builded lyra lib and model coeffs to linphone-sdk deployment
sudo cp lib/*.so ../linphone-sdk/cmake-build-relwithdebinfo/lib
sudo cp -r lyra/model_coeffs/ ../linphone-sdk/cmake-build-relwithdebinfo/lib/lyra_model_coeffs
