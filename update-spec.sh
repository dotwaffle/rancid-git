#!/bin/bash
#
# Update rpm spec file to use HEAD or any valid ref-spec

archive='no'
spec_file='rancid-git.spec'
program=$(basename $0)
export SHA=$(git rev-parse HEAD)
sha_short=$(git rev-parse --short HEAD)
version='2.3.8'
release='0'


usage() {
cat << EOF
$program [--help] [--commit [sha|tag]] [--archive]

  Update the spec file to point at the curret HEAD unless
  a sha or tag is specified.

    --help      Display this help

    --commit    Specify SHA or tag to put in the spec file
                If a tag is specified, it will be translated to a full SHA

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

create_archive() {
    git archive -o rancid-git-${version}-${sha_short}.tar.gz \
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

        * )
        echo -e "Syntax error\n"
        usage
        exit 1
    esac
    shift
done

update_sha_in_spec
