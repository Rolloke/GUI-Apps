#!/bin/bash

REPOSITORY_URL=$1
REPOSITORY=$2
SUB_FOLDER_TO_KEEP=$3
BASE_DIR=$4

echo "URL:                ${REPOSITORY_URL}"
echo "Repository:         ${REPOSITORY}"
echo "Subfolder to keep:  ${SUB_FOLDER_TO_KEEP}"
echo "Directory to store: ${BASE_DIR}"
echo "Extract subfolder of given repostiory and store it into given directory."
echo "The complete history of the original folder is preserved"
echo "and imported to the destination repository in the second step."
echo "continue (y, n)?"
read answer
if [ ${answer} == 'y' ]; then
echo "starting, get a coffee, this may last a little ;-)"
else
echo "aborting"
exit 1
fi

echo "cloning repository ${REPOSITORY_URL}"
git clone --branch dev --origin origin --progress -v ${REPOSITORY_URL}

if [ -d ${REPOSITORY} ]; then
   echo "got to ${REPOSITORY}"
   cd ${REPOSITORY}
else
    echo "wrong name or missing repository folder ${REPOSITORY}"
    exit 1
fi

echo "remove connection to origin"
git remote rm origin

if [ -d ${SUB_FOLDER_TO_KEEP} ]; then

    echo "remove connection to origin"
    git filter-branch --subdirectory-filter ${SUB_FOLDER_TO_KEEP} -- --all
else
    echo "wrong name or missing folder ${SUB_FOLDER_TO_KEEP} in repository ${REPOSITORY}"
    exit -1
fi

echo "reset"
git reset --hard
echo "compress"
git gc --aggressive 
echo "prune"
git prune
echo "clean"
git clean -fd 

mkdir -p ${BASE_DIR}
mv * ${BASE_DIR}

echo "please check the result in ${REPOSITORY} and finalize with:"
echo "git add ."
echo "git commit"
echo ""
echo "!! READY with PART 1 !!"

SECOND_SCRIPT="../import_from_repoA.sh"

echo "git remote add ${REPOSITORY} $(pwd)" > ${SECOND_SCRIPT}
echo "git pull ${REPOSITORY} dev --allow-unrelated-histories --no-rebase" >> ${SECOND_SCRIPT}
echo "git remote rm ${REPOSITORY}" >> ${SECOND_SCRIPT}
chmod +x ${SECOND_SCRIPT}

echo ""
echo "!! PROCEED with PART 2 !!"
echo "Steps to import the extracted subfolder ${SUB_FOLDER_TO_KEEP} of repository ${REPOSITORY}"
echo ""
echo "git clone <git destination repository url>"
echo "cd <destination repository directory>"
echo "copy script ${SECOND_SCRIPT} to destination repository and execute it"
echo "check the result and push it with:"
echo "git push"
echo ""
echo "HINT: you may havo to adapt the push rules in your destination repostory temporarily"
