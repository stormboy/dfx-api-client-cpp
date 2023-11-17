echo "echo Restoring environment" > "/Users/drmacdon/Development/nuralogix/libdfx2/conan/recipes/zulu-openjdk/all/test_package/deactivate_conanrunenv-x86_64.sh"
for v in JAVA_HOME JDK_HOME PATH
do
    is_defined="true"
    value=$(printenv $v) || is_defined="" || true
    if [ -n "$value" ] || [ -n "$is_defined" ]
    then
        echo export "$v='$value'" >> "/Users/drmacdon/Development/nuralogix/libdfx2/conan/recipes/zulu-openjdk/all/test_package/deactivate_conanrunenv-x86_64.sh"
    else
        echo unset $v >> "/Users/drmacdon/Development/nuralogix/libdfx2/conan/recipes/zulu-openjdk/all/test_package/deactivate_conanrunenv-x86_64.sh"
    fi
done


export JAVA_HOME="/Users/drmacdon/.conan2/p/zulu-d32b4979f19b6/p"
export JDK_HOME="/Users/drmacdon/.conan2/p/zulu-d32b4979f19b6/p"
export PATH="/Users/drmacdon/.conan2/p/zulu-d32b4979f19b6/p/bin:$PATH"