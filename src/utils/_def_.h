/**
 * utils/_def_.h
 *
 * Copyright (c) 2019-2021 Yuriy Lisovskiy
 *
 * Purpose: utility module's definitions.
 */

#pragma once

#include "../_def_.h"


// xw::utils
#define __UTILS_BEGIN__ __MAIN_NAMESPACE_BEGIN__ namespace utils {
#define __UTILS_END__ } __MAIN_NAMESPACE_END__


// xw::utils::cache
#define __CACHE_BEGIN__ __UTILS_BEGIN__ namespace cache {
#define __CACHE_END__ } __UTILS_END__

// xw::utils::cache::internal
#define __CACHE_INTERNAL_BEGIN__ __CACHE_BEGIN__ namespace internal {
#define __CACHE_INTERNAL_END__ } __CACHE_END__


// xw::utils::fn
#define __FUNCTIONAL_BEGIN__ __UTILS_BEGIN__ namespace fn {
#define __FUNCTIONAL_END__ } __UTILS_END__


// xw::utils_http
#define __UTILS_HTTP_BEGIN__ __MAIN_NAMESPACE_BEGIN__ namespace utils_http {
#define __UTILS_HTTP_END__ } __MAIN_NAMESPACE_END__

// xw::utils_http::internal
#define __UTILS_HTTP_INTERNAL_BEGIN__ __UTILS_HTTP_BEGIN__ namespace internal {
#define __UTILS_HTTP_INTERNAL_END__ } __UTILS_HTTP_END__
