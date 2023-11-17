
Conan V2 requires two profiles when building:

1. build profile (platform your building on)
2. host profile (platform your build is targetting)

Conan profiles can include other profiles both relatively and
if not a relative path it will attempt to resolve from the
default ~/.conan2/profiles folder.

The first encounter of a definition provides the value
used for configuring the profile making the profiles
somewhat hierarchical and is exploited in this
layout to minimize value setting.

The build-default and host-default files in this folder
are libdfx2 specific defaults. These are included by the
${os}/build-default and ${os}/host-default configurations.

The ${os}/build-default is identical on all platforms
except android in which the toolchains only run on x86_64
so there is a customization on android to force the
build arch=x86_64.

Emscripten follows the same pattern which could be leveraged
to support SIMD, Threads etc. as target archs.