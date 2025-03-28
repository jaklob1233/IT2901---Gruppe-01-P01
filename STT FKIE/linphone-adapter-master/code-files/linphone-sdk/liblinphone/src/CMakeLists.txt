############################################################################
# CMakeLists.txt
# Copyright (c) 2010-2023 Belledonne Communications SARL.
#
############################################################################
#
# This file is part of Liblinphone 
# (see https://gitlab.linphone.org/BC/public/liblinphone).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
############################################################################

set(LINK_LIBS
	bctoolbox
	${BELLESIP_TARGETNAME}
	belr

	# Speech-to-text
	speechtotextServer
	texttospeechServer
	mqtttransportServer
)
set(PUBLIC_LINK_LIBS
	ortp
	mediastreamer2
	belle-sip
)

#interface_libs is used in export cmake config file
if(APPLE)
	list(APPEND INTERFACE_LIBS ${APPLE_LIBS})
endif()

if(ENABLE_XML2)
	list(APPEND LINK_LIBS ${XML2_LIBRARIES})
endif()

list(APPEND INTERFACE_LIBS ${LINK_LIBS} xsd)

if(HAVE_DLOPEN)
	list(APPEND LINK_LIBS dl)
endif()

if (ENABLE_ADVANCED_IM)
	list(APPEND LINK_LIBS ${XercesC_TARGET})
	if (APPLE AND NOT IOS)
		list(APPEND APPLE_LIBS "-framework CoreServices")
	endif()
endif()

if (LIME_FOUND)
	list(APPEND PUBLIC_LINK_LIBS ${LIME_TARGETNAME})
endif ()
if(ENABLE_LIME OR ENABLE_LIME_X3DH)
	list(APPEND LINK_LIBS ${BZRTP_LIBRARIES})
endif()
if(ZLIB_FOUND)
	list(APPEND INTERFACE_LIBS z)
	if (ANDROID)
		# Starting NDK r21, libz.a has issues: https://github.com/android/ndk/issues/1179
		list(APPEND LINK_LIBS z)
	else()
		list(APPEND LINK_LIBS ${ZLIB_LIBRARIES})
	endif()
endif()
if (ENABLE_DB_STORAGE)
	if (SOCI_FOUND)
		#take care of the order  for static link
		if(APPLE OR ANDROID)
			list(APPEND INTERFACE_LIBS ${SOCI_sqlite3_PLUGIN})
			list(APPEND LINK_LIBS ${SOCI_sqlite3_PLUGIN})
		endif()
		list(APPEND INTERFACE_LIBS soci)
		list(APPEND LINK_LIBS ${SOCI_LIBRARIES})
	endif()
endif()
if(SQLITE3_FOUND)
	list(APPEND INTERFACE_LIBS sqlite3)
	list(APPEND PUBLIC_LINK_LIBS ${SQLITE3_LIBRARIES})
endif()
if(ICONV_FOUND)
	list(APPEND INTERFACE_LIBS iconv)
	list(APPEND LINK_LIBS ${ICONV_LIBRARIES})
endif()
if(ENABLE_TUNNEL)
	list(APPEND LINK_LIBS ${TUNNEL_LIBRARIES})
  	add_definitions(-DTUNNEL_ENABLED)
endif()

if(WIN32 AND NOT CMAKE_SYSTEM_NAME STREQUAL "WindowsPhone" AND NOT CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
	list(APPEND LINK_LIBS shlwapi)
endif()
if(INTL_FOUND)
	list(APPEND INTERFACE_LIBS intl)
	list(APPEND LINK_LIBS ${INTL_LIBRARIES})
endif()
if(BELCARD_FOUND)
	list(APPEND INTERFACE_LIBS belcard)
	list(APPEND LINK_LIBS belcard)
endif()
if(OPENLDAP_FOUND)
	list(APPEND LINK_LIBS ${OPENLDAP_LIBRARIES})
endif()
#define log domain for this part of code
add_definitions(-DBCTBX_LOG_DOMAIN="liblinphone")
if(ENABLE_LDAP)
	add_definitions(-DBUILD_LDAP)
endif()

if(ZXING_FOUND)
	list(APPEND LINK_LIBS ${ZXING_LIBRARIES})
endif()

if(TURBOJPEG_FOUND)
	list(APPEND LINK_LIBS ${TURBOJPEG_LIBRARIES})
endif()

set(LINPHONE_CXX_OBJECTS_PRIVATE_HEADER_FILES
	account/account.h
	account/account-params.h
	address/address.h
	address/address-parser.cpp
	alert/alert.h
	auth-info/auth-info.h
	auth-info/auth-stack.h
	c-wrapper/c-wrapper.h
	c-wrapper/internal/c-sal.h
	c-wrapper/internal/c-tools.h
	call/call-log.h
	call/call.h
	call/video-source/video-source-descriptor.h
	call/audio-device/audio-device.h
	call/audio-device/audio-device.cpp
	chat/chat-message/chat-message-p.h
	chat/chat-message/chat-message-reaction.h
	chat/chat-message/chat-message.h
	chat/chat-message/chat-message-listener.h
	chat/chat-message/imdn-message-p.h
	chat/chat-message/imdn-message.h
	chat/chat-message/is-composing-message.h
	chat/chat-message/notification-message-p.h
	chat/chat-message/notification-message.h
	chat/chat-room/chat-room-params.h
	chat/chat-room/abstract-chat-room-p.h
	chat/chat-room/abstract-chat-room.h
	chat/chat-room/basic-chat-room-p.h
	chat/chat-room/basic-chat-room.h
	chat/chat-room/chat-room-listener.h
	chat/chat-room/chat-room-p.h
	chat/chat-room/chat-room.h
	chat/cpim/cpim.h
	chat/cpim/header/cpim-core-headers.h
	chat/cpim/header/cpim-generic-header.h
	chat/cpim/header/cpim-header-p.h
	chat/cpim/header/cpim-header.h
	chat/cpim/message/cpim-message.h
	chat/cpim/parser/cpim-parser.h
	chat/encryption/encryption-engine.h
	chat/encryption/legacy-encryption-engine.h
	chat/ics/ics.h
	chat/ics/parser/ics-parser.h
	chat/modifier/chat-message-modifier.h
	chat/modifier/cpim-chat-message-modifier.h
	chat/modifier/encryption-chat-message-modifier.h
	chat/modifier/file-transfer-chat-message-modifier.h
	chat/modifier/multipart-chat-message-modifier.h
	chat/notification/imdn.h
	chat/notification/is-composing-listener.h
	chat/notification/is-composing.h
	conference/conference-params.h
	conference/conference-enums.h
	conference/conference-id.h
	conference/conference-info.h
	conference/conference-scheduler.h
	conference/conference-listener.h
	conference/notify-conference-listener.h
	conference/conference.h
	conference/local-conference.h
	conference/params/call-session-params-p.h
	conference/params/call-session-params.h
	conference/params/media-session-params-p.h
	conference/params/media-session-params.h
	conference/participant-device.h
	conference/participant-imdn-state-p.h
	conference/participant-imdn-state.h
	conference/participant.h
	conference/remote-conference.h
	conference/session/call-session-listener.h
	conference/session/call-session-p.h
	conference/session/call-session.h
	conference/session/media-session.h
	conference/session/streams.h
	conference/session/port-config.h
	conference/session/tone-manager.h
	conference/session/ms2-streams.h
	conference/session/media-description-renderer.h
	conference/session/mixers.h
	containers/lru-cache.h
	content/content-disposition.h
	content/content-manager.h
	content/content-p.h
	content/content-type.h
	content/content.h
	content/file-content.h
	content/file-transfer-content.h
	content/header/header-p.h
	content/header/header-param.h
	content/header/header.h
	core/core-accessor.h
	core/core-listener.h
	core/core-p.h
	core/core.h
	core/paths/paths.h
	core/platform-helpers/platform-helpers.h
	core/shared-core-helpers/shared-core-helpers.h
	db/abstract/abstract-db-p.h
	db/abstract/abstract-db.h
	db/internal/statements.h
	db/main-db-event-key.h
	db/main-db-key-p.h
	db/main-db-key.h
	db/main-db-p.h
	db/main-db.h
	dial-plan/dial-plan.h
	dictionary/dictionary.h
	enums.h
	event/event.h
	event/event-publish.h
	event/event-subscribe.h
	event-log/conference/conference-call-event.h
	event-log/conference/conference-event-p.h
	event-log/conference/conference-event.h
	event-log/conference/conference-notified-event-p.h
	event-log/conference/conference-notified-event.h
	event-log/conference/conference-ephemeral-message-event.h
	event-log/conference/conference-participant-device-event.h
	event-log/conference/conference-participant-event-p.h
	event-log/conference/conference-participant-event.h
	event-log/conference/conference-security-event.h
	event-log/conference/conference-subject-event.h
	event-log/conference/conference-available-media-event.h
	event-log/event-log-p.h
	event-log/event-log.h
	event-log/events.h
	factory/factory.h
	friend/friend_phone_number.h
	hacks/hacks.h
	ldap/ldap.h
	ldap/ldap-config-keys.h
	ldap/ldap-params.h
	logger/logger.h
	nat/ice-service.h
	nat/stun-client.h
	nat/nat-policy.h
	object/app-data-container.h
	object/base-object-p.h
	object/base-object.h
	object/clonable-object-p.h
	object/clonable-object.h
	object/clonable-shared-pointer.h
	object/object-head-p.h
	object/object-head.h
	object/object-p.h
	object/object.h
	object/property-container.h
	object/singleton.h
	payload-type/payload-type.h
	push-notification-message/push-notification-message.h
	push-notification/push-notification-config.h
	recorder/recorder.h
	recorder/recorder-params.h
	sal/sal.h
	sal/sal_stream_bundle.h
	sal/sal_stream_description.h
	sal/sal_stream_configuration.h
	sal/sal_media_description.h
	sal/params/sal_media_description_params.h
	sal/offeranswer.h
	sal/potential_config_graph.h
	search/search-async-data.h
	search/magic-search-p.h
	search/magic-search.h
	search/search-request.h
	search/search-result.h
	signal-information/signal-information.h
	utils/background-task.h
	utils/custom-params.h
	utils/general-internal.h
	utils/payload-type-handler.h
	utils/if-addrs.h
	variant/variant.h
	variant/variant-impl.h
)

if(ENABLE_ADVANCED_IM)
	list(APPEND LINPHONE_CXX_OBJECTS_PRIVATE_HEADER_FILES
		chat/chat-room/basic-to-client-group-chat-room.h
		chat/chat-room/client-group-chat-room-p.h
		chat/chat-room/client-group-chat-room.h
		chat/chat-room/client-group-to-basic-chat-room.h
		chat/chat-room/proxy-chat-room-p.h
		chat/chat-room/proxy-chat-room.h
		chat/chat-room/server-group-chat-room-p.h
		chat/chat-room/server-group-chat-room.h
		conference/handlers/local-audio-video-conference-event-handler.h
		conference/handlers/local-conference-event-handler.h
		conference/handlers/local-conference-list-event-handler.h
		conference/handlers/remote-conference-event-handler-base.h
		conference/handlers/remote-conference-event-handler.h
		conference/handlers/remote-conference-list-event-handler.h
		xml/conference-info.h
		xml/conference-info-linphone-extension.h
		xml/imdn.h
		xml/is-composing.h
		xml/linphone-imdn.h
		xml/publish-linphone-extension.h
		xml/resource-lists.h
		xml/rlmi.h
		xml/xml.h
	)
endif()

if(ENABLE_LDAP)
	list(APPEND LINPHONE_CXX_OBJECTS_PRIVATE_HEADER_FILES
		ldap/ldap-contact-fields.h
		ldap/ldap-contact-provider.h
		ldap/ldap-contact-search.h
		)
endif()

if(ENABLE_DB_STORAGE)
	list(APPEND LINPHONE_CXX_OBJECTS_PRIVATE_HEADER_FILES
		db/internal/db-transaction.h
		db/session/db-session.h
	)
endif()

set(LINPHONE_CXX_OBJECTS_SOURCE_FILES
	account/account.cpp
	account/account-params.cpp
	account_creator/utils.cpp
	account_creator/service.cpp
	account_creator/main.cpp
	account_creator/connector_xmlrpc.cpp
	address/address.cpp
	address/address-parser.cpp
	alert/alert.cpp
	auth-info/auth-info.cpp
	auth-info/auth-stack.cpp
	c-wrapper/c-wrapper.cpp
	c-wrapper/api/c-digest-authentication-policy.cpp
	c-wrapper/api/c-account.cpp
	c-wrapper/api/c-account-cbs.cpp
	c-wrapper/api/c-account-params.cpp
	c-wrapper/api/c-address.cpp
	c-wrapper/api/c-alert-cbs.cpp
	c-wrapper/api/c-alert.cpp
	c-wrapper/api/c-audio-device.cpp
	c-wrapper/api/c-auth-info.cpp
	c-wrapper/api/c-call-cbs.cpp
	c-wrapper/api/c-call-log.cpp
	c-wrapper/api/c-call-params.cpp
	c-wrapper/api/c-call-stats.cpp
	c-wrapper/api/c-call.cpp
	c-wrapper/api/c-chat-message-cbs.cpp
	c-wrapper/api/c-chat-message-reaction.cpp
	c-wrapper/api/c-chat-message.cpp
	c-wrapper/api/c-chat-room-params.cpp
	c-wrapper/api/c-chat-room-cbs.cpp
	c-wrapper/api/c-chat-room.cpp
	c-wrapper/api/c-content.cpp
	c-wrapper/api/c-conference-cbs.cpp
	c-wrapper/api/c-conference-info.cpp
	c-wrapper/api/c-conference-scheduler.cpp
	c-wrapper/api/c-conference.cpp
	c-wrapper/api/c-core.cpp
	c-wrapper/api/c-dial-plan.cpp
	c-wrapper/api/c-dictionary.cpp
	c-wrapper/api/c-event-cbs.cpp
	c-wrapper/api/c-event-log.cpp
	c-wrapper/api/c-event.cpp
	c-wrapper/api/c-factory.cpp
	c-wrapper/api/c-friend-phone-number.cpp
	c-wrapper/api/c-ldap.cpp
	c-wrapper/api/c-ldap-params.cpp
	c-wrapper/api/c-magic-search.cpp
	c-wrapper/api/c-magic-search-cbs.cpp
	c-wrapper/api/c-participant.cpp
	c-wrapper/api/c-participant-device.cpp
	c-wrapper/api/c-participant-device-cbs.cpp
	c-wrapper/api/c-participant-device-identity.cpp
	c-wrapper/api/c-participant-imdn-state.cpp
	c-wrapper/api/c-participant.cpp
	c-wrapper/api/c-payload-type.cpp
	c-wrapper/api/c-push-notification-message.cpp
	c-wrapper/api/c-push-notification-config.cpp
	c-wrapper/api/c-recorder.cpp
	c-wrapper/api/c-recorder-params.cpp
	c-wrapper/api/c-search-result.cpp
	c-wrapper/api/c-signal-information.cpp
	c-wrapper/api/c-video-source-descriptor.cpp
	c-wrapper/api/c-nat-policy.cpp
	c-wrapper/internal/c-sal.cpp
	c-wrapper/internal/c-tools.cpp
	call/call-log.cpp
	call/call.cpp
	call/video-source/video-source-descriptor.cpp
	chat/chat-message/chat-message-reaction.cpp
	chat/chat-message/chat-message.cpp
	chat/chat-message/imdn-message.cpp
	chat/chat-message/is-composing-message.cpp
	chat/chat-message/notification-message.cpp
	chat/chat-room/abstract-chat-room.cpp
	chat/chat-room/basic-chat-room.cpp
	chat/chat-room/chat-room.cpp
	chat/chat-room/chat-room-params.cpp
	chat/encryption/legacy-encryption-engine.cpp
	chat/modifier/encryption-chat-message-modifier.cpp
	chat/modifier/file-transfer-chat-message-modifier.cpp
	chat/modifier/multipart-chat-message-modifier.cpp
	chat/notification/imdn.cpp
	chat/notification/is-composing.cpp
	conference/conference-params.cpp
	conference/conference-enums.cpp
	conference/conference-id.cpp
	conference/conference-info.cpp
	conference/conference-scheduler.cpp
	conference/conference.cpp
	conference/conference-interface.cpp
	conference/notify-conference-listener.cpp
	conference/local-conference.cpp
	conference/params/call-session-params.cpp
	conference/params/media-session-params.cpp
	conference/participant-device.cpp
	conference/participant-imdn-state.cpp
	conference/participant.cpp
	conference/remote-conference.cpp
	conference/session/call-session.cpp
	conference/session/media-session.cpp
	conference/session/tone-manager.cpp
	conference/session/media-description-renderer.cpp
	conference/session/stream.cpp
	conference/session/streams-group.cpp
	conference/session/ms2-stream.cpp
	conference/session/audio-stream.cpp
	conference/session/rtt-stream.cpp
	conference/session/media-session.cpp
	conference/session/mixer-session.cpp
	conference/session/audio-mixer.cpp
	content/content-disposition.cpp
	content/content-manager.cpp
	content/content-type.cpp
	content/content.cpp
	content/file-content.cpp
	content/file-transfer-content.cpp
	content/header/header-param.cpp
	content/header/header.cpp
	core/core-accessor.cpp
	core/core-call.cpp
	core/core-chat-room.cpp
	core/core.cpp
	core/paths/paths.cpp
	core/platform-helpers/platform-helpers.cpp
	core/shared-core-helpers/shared-core-helpers.cpp
	db/abstract/abstract-db.cpp
	db/internal/statements.cpp
	db/main-db-event-key.cpp
	db/main-db-key.cpp
	db/main-db.cpp
	dial-plan/dial-plan.cpp
	dictionary/dictionary.cpp
	event/event.cpp
	event/event-publish.cpp
	event/event-subscribe.cpp
	event-log/conference/conference-call-event.cpp
	event-log/conference/conference-chat-message-event.cpp
	event-log/conference/conference-event.cpp
	event-log/conference/conference-notified-event.cpp
	event-log/conference/conference-participant-device-event.cpp
	event-log/conference/conference-participant-event.cpp
	event-log/conference/conference-security-event.cpp
	event-log/conference/conference-subject-event.cpp
	event-log/conference/conference-available-media-event.cpp
	event-log/conference/conference-ephemeral-message-event.cpp
	event-log/event-log.cpp
	factory/factory.cpp
	friend/friend_phone_number.cpp
	hacks/hacks.cpp
	ldap/ldap.cpp
	ldap/ldap-config-keys.cpp
	ldap/ldap-params.cpp
	logger/logger.cpp
	nat/ice-service.cpp
	nat/stun-client.cpp
	nat/nat-policy.cpp
	object/app-data-container.cpp
	object/base-object.cpp
	object/clonable-object.cpp
	object/object.cpp
	object/property-container.cpp
	payload-type/payload-type.cpp
	push-notification-message/push-notification-message.cpp
	push-notification/push-notification-config.cpp
	recorder/recorder.cpp
	recorder/recorder-params.cpp
	sal/call-op.cpp
	sal/event-op.cpp
	sal/message-op.cpp
	sal/op.cpp
	sal/presence-op.cpp
	sal/refer-op.cpp
	sal/register-op.cpp
	sal/sal.cpp
	sal/sal_stream_bundle.cpp
	sal/sal_stream_description.cpp
	sal/sal_stream_configuration.cpp
	sal/sal_media_description.cpp
	sal/params/sal_media_description_params.cpp
	sal/offeranswer.cpp
	sal/potential_config_graph.cpp
	search/magic-search.cpp
	search/search-async-data.cpp
	search/search-request.cpp
	search/search-result.cpp
	signal-information/signal-information.cpp
	utils/background-task.cpp
	utils/custom-params.cpp
	utils/fs.cpp
	utils/general.cpp
	utils/payload-type-handler.cpp
	utils/utils.cpp
	utils/if-addrs.cpp
	utils/version.cpp
)

if(ENABLE_LDAP)
	list(APPEND LINPHONE_CXX_OBJECTS_SOURCE_FILES
		ldap/ldap-contact-fields.cpp
		ldap/ldap-contact-provider.cpp
		ldap/ldap-contact-search.cpp
		)
endif()

if (ENABLE_FLEXIAPI)
	list(APPEND LINPHONE_CXX_OBJECTS_SOURCE_FILES
		account_creator/connector_flexiapi.cpp
		FlexiAPIClient.cc
	)
endif()

if(ENABLE_VIDEO)
	list(APPEND LINPHONE_CXX_OBJECTS_SOURCE_FILES
		conference/session/video-mixer.cpp
		conference/session/video-stream.cpp
	)
endif()

if(ENABLE_ADVANCED_IM)
	list(APPEND LINPHONE_CXX_OBJECTS_SOURCE_FILES
		chat/chat-room/basic-to-client-group-chat-room.cpp
		chat/chat-room/client-group-chat-room.cpp
		chat/chat-room/client-group-to-basic-chat-room.cpp
		chat/chat-room/proxy-chat-room.cpp
		chat/chat-room/server-group-chat-room.cpp
		chat/modifier/cpim-chat-message-modifier.cpp
		conference/handlers/local-conference-event-handler.cpp
		conference/handlers/local-audio-video-conference-event-handler.cpp
		conference/handlers/local-conference-list-event-handler.cpp
		conference/handlers/remote-conference-event-handler.cpp
		conference/handlers/remote-conference-list-event-handler.cpp
		xml/conference-info.cpp
		xml/conference-info-linphone-extension.cpp
		xml/imdn.cpp
		xml/is-composing.cpp
		xml/linphone-imdn.cpp
		xml/publish-linphone-extension.cpp
		xml/resource-lists.cpp
		xml/rlmi.cpp
		xml/xml.cpp
		chat/cpim/header/cpim-core-headers.cpp
		chat/cpim/header/cpim-generic-header.cpp
		chat/cpim/header/cpim-header.cpp
		chat/cpim/message/cpim-message.cpp
		chat/cpim/parser/cpim-parser.cpp
		chat/ics/ics.cpp
		chat/ics/parser/ics-parser.cpp
	)
	list(APPEND LINPHONE_CXX_OBJECTS_PRIVATE_HEADER_FILES chat/encryption/lime-x3dh-server-engine.h)
	list(APPEND LINPHONE_CXX_OBJECTS_SOURCE_FILES chat/encryption/lime-x3dh-server-engine.cpp)
endif()

if (ENABLE_DB_STORAGE)
	list(APPEND LINPHONE_CXX_OBJECTS_SOURCE_FILES db/session/db-session.cpp)
endif()

set(LINPHONE_OBJC_SOURCE_FILES)
if (APPLE)
	list(APPEND LINPHONE_OBJC_SOURCE_FILES core/paths/paths-apple.mm)
	list(APPEND LINPHONE_CXX_OBJECTS_PRIVATE_HEADER_FILES core/paths/paths-apple.h)
	list(APPEND LINPHONE_OBJC_SOURCE_FILES core/platform-helpers/ios-platform-helpers.mm)
	list(APPEND LINPHONE_OBJC_SOURCE_FILES core/platform-helpers/mac-platform-helpers.mm)
	list(APPEND LINPHONE_OBJC_SOURCE_FILES core/platform-helpers/mac-platform-helpers.h)
	list(APPEND LINPHONE_OBJC_SOURCE_FILES core/shared-core-helpers/ios-shared-core-helpers.mm)
	if (IOS)
		list(APPEND LINPHONE_OBJC_SOURCE_FILES core/app/ios-app-delegate.h)
		list(APPEND LINPHONE_OBJC_SOURCE_FILES core/app/ios-app-delegate.mm)
	endif()
elseif (ANDROID)
	list(APPEND LINPHONE_CXX_OBJECTS_SOURCE_FILES core/paths/paths-android.cpp core/platform-helpers/android-platform-helpers.cpp)
	list(APPEND LINPHONE_CXX_OBJECTS_PRIVATE_HEADER_FILES core/paths/paths-android.h)
elseif (WIN32)
	list(APPEND LINPHONE_CXX_OBJECTS_SOURCE_FILES core/paths/paths-windows.cpp)
	list(APPEND LINPHONE_CXX_OBJECTS_PRIVATE_HEADER_FILES core/paths/paths-windows.h)
elseif (UNIX)
	list(APPEND LINPHONE_CXX_OBJECTS_SOURCE_FILES core/paths/paths-linux.cpp)
	list(APPEND LINPHONE_CXX_OBJECTS_PRIVATE_HEADER_FILES core/paths/paths-linux.h)
endif ()

if(LIME_FOUND)
	list(APPEND LINPHONE_CXX_OBJECTS_PRIVATE_HEADER_FILES chat/encryption/lime-x3dh-encryption-engine.h)
	list(APPEND LINPHONE_CXX_OBJECTS_SOURCE_FILES chat/encryption/lime-x3dh-encryption-engine.cpp)
endif()

set(LINPHONE_CXX_OBJECTS_INCLUDE_DIRS
	${LIBXSD_INCLUDE_DIRS}
	${SOCI_INCLUDE_DIRS}
	${SOCI_MYSQL_INCLUDES}
)

set(LINPHONE_PRIVATE_HEADER_FILES)
foreach (header ${LINPHONE_CXX_OBJECTS_PRIVATE_HEADER_FILES})
	list(APPEND LINPHONE_PRIVATE_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${header}")
endforeach ()
set(LINPHONE_PRIVATE_HEADER_FILES ${LINPHONE_PRIVATE_HEADER_FILES} PARENT_SCOPE)

		if (ANDROID AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
	list(APPEND STRICT_OPTIONS_CXX "-Wno-attributes")
endif()

# Fix for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105562 and https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105616

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
		AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 12
		AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14)
	set_source_files_properties(sal/potential_config_graph.cpp
			utils/version.cpp
			PROPERTIES COMPILE_OPTIONS "-Wno-error=maybe-uninitialized")
endif()

bc_apply_compile_flags(LINPHONE_CXX_OBJECTS_SOURCE_FILES STRICT_OPTIONS_CPP STRICT_OPTIONS_CXX)
bc_apply_compile_flags(LINPHONE_OBJC_SOURCE_FILES STRICT_OPTIONS_CPP STRICT_OPTIONS_OBJC)

if ((CMAKE_COMPILER_IS_GNUCC AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
		OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "10.0.0")
		OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
	set_property(SOURCE
		xml/conference-info.cpp
		xml/publish-linphone-extension.cpp
		xml/resource-lists.cpp
		xml/rlmi.cpp
		APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-error=deprecated-copy"
	)
endif()

if(BUILD_SHARED_LIBS)
	set(SWIFT_MODULE_FILES
		"${PROJECT_SOURCE_DIR}/wrappers/swift/Modules/module.modulemap"
		"${PROJECT_SOURCE_DIR}/wrappers/swift/Modules/swift-umbrella.h"
	)
	set(LINPHONE_RESOURCES ${GRAMMAR_FILES} ${SOUND_FILES} ${RING_FILES} ${ROOTCA_FILE})
	set_source_files_properties(${SWIFT_MODULE_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Modules)
endif()

add_library(liblinphone ${LINPHONE_RESOURCES} ${SWIFT_MODULE_FILES} ${LINPHONE_HEADER_FILES}
	${LINPHONE_CXX_OBJECTS_PRIVATE_HEADER_FILES} ${LINPHONE_CXX_OBJECTS_SOURCE_FILES} ${LINPHONE_OBJC_SOURCE_FILES}
	$<TARGET_OBJECTS:liblinphone-coreapi>
)

#APPLE_LIBS is mandatory for apple compilation
if(APPLE)
	list(APPEND APPLE_LIBS "-framework CFNetwork -framework Foundation -framework AVFoundation -framework SystemConfiguration -framework CoreLocation")
	if(IOS)
		list(APPEND APPLE_LIBS "-framework UIKit -framework PushKit -framework CallKit")
	endif()
endif()

if(APPLE)
	set_target_properties(liblinphone PROPERTIES OUTPUT_NAME linphone)
else()
	set_target_properties(liblinphone PROPERTIES OUTPUT_NAME liblinphone)
endif()
set_target_properties(liblinphone PROPERTIES PREFIX "")
set_target_properties(liblinphone PROPERTIES LINKER_LANGUAGE CXX)
if(NOT ANDROID)
	# Do not version shared library on Android
	set_target_properties(liblinphone PROPERTIES SOVERSION ${LINPHONE_SO_VERSION})
endif()
target_include_directories(liblinphone PUBLIC
	$<INSTALL_INTERFACE:include/>
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/coreapi/>
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/>
	PRIVATE ${LINPHONE_INCLUDE_DIRS} ${LINPHONE_CXX_OBJECTS_INCLUDE_DIRS}
)
target_link_libraries(liblinphone
	PUBLIC ${PUBLIC_LINK_LIBS}
	PRIVATE ${LINK_LIBS}
)
if(ENABLE_FLEXIAPI)
	target_link_libraries(liblinphone PUBLIC ${JSONCPP_LIBRARIES})
endif()

### Speech-to-Text
## STT
add_dependencies(liblinphone speechtotextServer)
target_link_libraries(liblinphone PRIVATE speechtotextServer)

## TTS
add_dependencies(liblinphone texttospeechServer)
target_link_libraries(liblinphone PRIVATE texttospeechServer)

## Mqtttransport
add_dependencies(liblinphone mqtttransportServer)
target_link_libraries(liblinphone PRIVATE mqtttransportServer)


if(APPLE)
	target_link_libraries(liblinphone PUBLIC ${APPLE_LIBS})
endif()

if(ICONV_FOUND)
	if(APPLE)
		# Prevent conflict between the system iconv.h header and the one from macports.
		target_compile_options(liblinphone PRIVATE "-include" "${ICONV_INCLUDE_DIRS}/iconv.h")
	else()
		target_include_directories(liblinphone PRIVATE ${ICONV_INCLUDE_DIRS})
	endif()
endif()

if(BUILD_SHARED_LIBS)
	if(MSVC)
		set_target_properties(liblinphone PROPERTIES PDB_NAME liblinphone)
	endif()
	target_compile_definitions(liblinphone PRIVATE "-DLINPHONE_EXPORTS")
	if(APPLE)
		if(IOS)
			set(MIN_OS ${LINPHONE_IOS_DEPLOYMENT_TARGET})
		else()
			set(MIN_OS ${CMAKE_OSX_DEPLOYMENT_TARGET})
		endif()

		set_source_files_properties(${LINPHONE_ROOT_HEADER_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Headers/")
		set_source_files_properties(${LINPHONE_C_API_HEADER_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Headers/api")
		set_source_files_properties(${LINPHONE_ENUMS_HEADER_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Headers/enums")
		set_source_files_properties(${LINPHONE_UTILS_HEADER_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Headers/utils")
		list(GET LINPHONE_ROOT_HEADER_FILES 0 one_header_file)
		set_target_properties(liblinphone PROPERTIES
			FRAMEWORK TRUE
			MACOSX_FRAMEWORK_IDENTIFIER org.linphone.linphone
			MACOSX_FRAMEWORK_INFO_PLIST "${PROJECT_SOURCE_DIR}/build/osx/Info.plist.in"
			RESOURCE "${LINPHONE_RESOURCES}"
			PUBLIC_HEADER "${one_header_file}"
		)
	endif()

	if(WIN32)
		# Export Xerces and Soci symbols.
		if(ENABLE_DB_STORAGE)
			target_compile_definitions(liblinphone PRIVATE "-DDLL_EXPORT" "-DSOCI_DLL")
		endif()
	elseif(ANDROID)
		if (SUPPORT_FOUND)
			target_link_libraries(liblinphone PUBLIC "log" ${SUPPORT_LIBRARIES} ${CPUFEATURES_LIBRARIES})
		else()
			target_link_libraries(liblinphone PUBLIC "log" ${CPUFEATURES_LIBRARIES})
		endif()
	endif()
	if(MSVC)
		if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
			install(FILES $<TARGET_PDB_FILE:liblinphone>
				DESTINATION ${CMAKE_INSTALL_BINDIR}
				PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)
		endif()
	endif()
endif()

install(TARGETS liblinphone EXPORT ${EXPORT_TARGETS_NAME}Targets
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	FRAMEWORK DESTINATION Frameworks
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

if(ROOTCA_FILE)
	install(FILES ${ROOTCA_FILE}
		DESTINATION "${CMAKE_INSTALL_DATADIR}/linphone"
		PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
	)
else()
	message(FATAL_ERROR "Rootca is missing and cannot be installed")
endif()

if(NOT APPLE OR IOS)
	if(GRAMMAR_FILES)
		install(FILES ${GRAMMAR_FILES}
			DESTINATION "${CMAKE_INSTALL_DATADIR}/belr/grammars"
			PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
		)
	endif()
	if(ENABLE_ASSETS AND SOUND_FILES)
		install(FILES ${SOUND_FILES}
			DESTINATION "${CMAKE_INSTALL_DATADIR}/sounds/linphone"
			PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
		)
	endif()

	if(RING_FILES)
		install(FILES ${RING_FILES}
			DESTINATION "${CMAKE_INSTALL_DATADIR}/sounds/linphone/rings"
			PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
		)
	endif()
endif()
