#!/bin/bash
#
# Update rpm spec file to use HEAD or any valid ref-spec

archive='no'
spec_file='rancid-git.spec'
program=$(basename $0)
sha_short=$(git rev-parse --short HEAD)
export SHA=$(git rev-parse HEAD)
export VERSION='2.3.8'
export RELEASE='0'


usage() {
cat << EOF
$program [--help] [--commit [sha|tag]] [--version [number]]
         [--release [number]] [--archive]

  Update the spec file to point at the curret HEAD unless
  a sha or tag is specified.

    --help      Display this help

    --commit    Specify SHA or tag to put in the spec file
                If a tag is specified, it will be translated to a full SHA

    --version   Specify version number to set in spec file (currently v$version)

    --release   Specify release number to set in spec file (currently $release)

    --archive   Create tar.gz archive

EOF
exit 0
}

update_sha_in_spec() {
    perl -pi -e 's/(^%global commit )[0-9a-fA-F]{5,40}/$1$ENV{SHA}/g' $spec_file

    if [[ $archive == "yes" ]]; then
        create_archive
    fi
}

update_version_in_spec() {
    perl -pi -e 's/(^Version: )(\d(\.)?)+(\d)?/$1$ENV{VERSION}/g' $spec_file
}

update_release_in_spec() {
    perl -pi -e 's/(^Release: )(\d)+/$1$ENV{RELEASE}/g' $spec_file
}

create_archive() {
    git archive -o rancid-git-${SHA}.tar.gz \
                --prefix=rancid-git-${SHA}/ ${SHA}
}

case $1 in
    --help )
        usage
        ;;
esac

# Loop through command line arguments
while [ "$1" ]; do
    case $1 in
        --archive )
            archive='yes'
            ;;

        --commit )
            ref="$2"

            # Validate ref-spec input
            git rev-parse $ref > /dev/null 2>&1
            if [[ $? != 0 ]]; then
                echo "\"$ref\" is not a valid ref-spec"
                exit 2
            fi

            export SHA=$(git rev-parse $ref)
            sha_short=$(git rev-parse --short $ref)
            shift
            ;;

        --version )
            export VERSION="$2"

            # Validate input
            if [[ $VERSION =~ ^[0-9](\.)? ]]; then
                update_version_in_spec
            else
                echo "\"$VERSION\" is not a valid version number"
                exit 3
            fi
            shift
            ;;

        --release )
            export RELEASE="$2"
            update_release_in_spec
            shift
            ;;

        * )
        echo -e "Syntax error\n"
        usage
        exit 1
    esac
    shift
done

update_sha_in_spec
