#!/usr/bin/env sh

if [ -f "./.release" ]; then
	export RELEASED=1
	export RELENG="prepared"
else
	export RELENG="built"
fi
rm -f ./.release 2> /dev/null

get_git_info()
{
	if command command true 2> /dev/null 1>&2; then
		if command git version 2> /dev/null 1>&2; then
			GITTEST=$(git version 2> /dev/null)
			# shellcheck disable=SC2236
			if [ -n "${GITTEST}" ] && [ ! -z "${GITTEST}" ]; then
				BRANCH=$(git branch --show-current 2> /dev/null)
				if [ -n "${RELEASED}" ] && [ ! -z "${RELEASED}" ]; then
					GITVER=$(git describe --tags --always 2> /dev/null \
						| cut -d "-" -f 1 2> /dev/null)
				else
					GITVER=$(git describe --tags --dirty --broken --long --always \
						2> /dev/null)
				fi

				if [ ! -n "${BRANCH}" ] || [ -z "${BRANCH}" ]; then
					BRANCH="nobranch"
				fi

				if [ ! -n "${RELEASED}" ] && [ -z "${RELEASED}" ]; then
					if [ -n "${GITVER}" ] && [ ! -z "${GITVER}" ]; then
						GIT_OUT=" ${GITVER}-${BRANCH}"
					fi
				fi
				if [ -n "${GITVER}" ] && [ ! -z "${GITVER}" ]; then
					GIT_OUT=" ${GITVER}"
				fi
			fi
		fi
	fi

	GIT_SOURCE_INFO="DUMA${GIT_OUT}"
	GIT_SOURCE_XFRM=$(printf '%s\n' "${GIT_SOURCE_INFO}" \
		| sed -e 's/\VERSION_//' -e 's/_/\./g' 2> /dev/null)

	# shellcheck disable=SC2236
	if [ -n "${GIT_SOURCE_XFRM}" ] && [ ! -z "${GIT_SOURCE_XFRM}" ]; then
		printf '%s\n' "${GIT_SOURCE_XFRM}"
	else
		printf '%s\n' "${GIT_SOURCE_INFO}"
	fi
}

get_utc_date()
{
	UTC_DATE=$(TZ=UTC date -u "+%D %T" 2> /dev/null)
	# shellcheck disable=SC2236
	if [ -n "${UTC_DATE}" ] && [ ! -z "${UTC_DATE}" ]; then
		UTC_DATE_INFO=", ${RELENG} ${UTC_DATE}"
	else
		UTC_DATE_INFO=""
	fi
	printf '%s\n' "${UTC_DATE_INFO}"
}

BUILD_VER=$(get_git_info)
BUILD_UTC=$(get_utc_date)

printf '%s\n' "// Auto-generated git information."
printf '%s\n' "#define GIT_SOURCE_VERSION \"${BUILD_VER}${BUILD_UTC} (\""
