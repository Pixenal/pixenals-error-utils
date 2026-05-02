/* 
SPDX-FileCopyrightText: 2025 Caleb Dawson
SPDX-License-Identifier: Apache-2.0
*/

#pragma once
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum PixErr {
	PIX_ERR_NOT_SET,
	PIX_ERR_SUCCESS,
	PIX_ERR_ERROR,
	PIX_ERR_QUIET,
	PIX_ERR_IO,
	PIX_ERR_DEP
} PixErr;

#ifndef WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif
static const char *pixErrAssertStr = "PIX ASSERT in %s, MESSAGE: %s\n";
static const char *pixErrThrowStr = "PIX ERR ERROR THROWN IN %s, IDX: %d, MESSAGE: %s\n";
static const char *pixErrRetStr = "PIX ERR ERROR THROWN IN %s, MESSAGE: %s\n";
static const char *pixErrErrStr =
	"PIX ERROR THROWN IN %s, IDX: %d, CODE: %d, CONDITION (%s) WAS %s, MESSAGE: %s\n";
static
const char *pixErrWarnStr = "PIX WARNING IN %s, CONDITION (%s) WAS %s, MESSAGE: %s\n";
static const char *pixErrReportStr = "PIX REPORT IN %s, MESSAGE: %s\n";
#ifndef WIN32
#pragma GCC diagnostic pop
#endif

#ifdef _DEBUG
#define PIX_ERR_ASSERT(message, condition) \
	if (!(condition)) \
	printf(pixErrAssertStr, __func__, message); \
	assert(condition);
#else
#define PIX_ERR_ASSERT(message, condition)\
	if (!(condition)) \
	printf(pixErrAssertStr, __func__, message); \
	assert(condition);
#endif

static inline
void pixerrPrintWarn(
	bool isNotCondition,
	const char *pCondition,
	const char *pMessage,
	const char *pFunc
) {
	char *isNotConditionStr = isNotCondition ? "false" : "true";
	printf(pixErrWarnStr,
		pFunc,
		pCondition,
		isNotConditionStr,
		pMessage
	);
}

static inline
void pixerrPrintError(
	PixErr err,
	int32_t idx,
	bool isNotCondition,
	const char *pCondition,
	const char *pMessage,
	const char *pFunc
) {
	if (err == PIX_ERR_QUIET) {
		return;
	}
	char *isNotConditionStr = isNotCondition ? "false" : "true";
	printf(pixErrErrStr,
		pFunc,
		idx,
		err,
		pCondition,
		isNotConditionStr,
		pMessage
	);
}

#define PIX_ERR_REPORT(message) printf(pixErrReportStr, __func__, message)

#define PIX_ERR_WARN_IFNOT_COND(condition, message)\
{\
	bool isNotCondition = !(condition);\
	if (isNotCondition) { \
		pixerrPrintWarn(isNotCondition, #condition, #message, __func__);\
	}\
}

//TODO add ifdef for putting assert inside PIX_ERR_THROW macros for debug builds,
// or if specified with a define
#define PIX_ERR_THROW_IFNOT_COND(err, condition, message, idx) \
{\
	bool isNotCondition = !(condition);\
	if (isNotCondition) {\
		err = PIX_ERR_ERROR;\
	}\
	if (err != PIX_ERR_SUCCESS) { \
		pixerrPrintError(err, idx, isNotCondition, #condition, #message, __func__);\
		goto handle_error_##idx; \
	}\
}

#define PIX_ERR_THROW_IFNOT(err, message, idx)\
	if (err != PIX_ERR_SUCCESS) { \
		pixerrPrintError(err, idx, true, "'N/A'", #message, __func__);\
		goto handle_error_##idx; \
	}

#define PIX_ERR_RETURN_IFNOT_COND(err, condition, message)\
{\
	bool isNotCondition = !(condition);\
	if (isNotCondition) {\
		err = PIX_ERR_ERROR;\
	}\
	if (err != PIX_ERR_SUCCESS) { \
		pixerrPrintError(err, -1, isNotCondition, #condition, #message, __func__);\
		return err;\
	}\
}

#define PIX_ERR_RETURN_QUIET_IFNOT_COND(err, condition, message)\
{\
	if (err != PIX_ERR_SUCCESS || !(condition)) { \
		err = PIX_ERR_QUIET;\
		return err;\
	}\
}

#define PIX_ERR_RETURN_IFNOT(err, message)\
	if (err != PIX_ERR_SUCCESS) { \
		pixerrPrintError(err, -1, true, "'N/A'", #message, __func__);\
		return err;\
	}

#define PIX_ERR_THROW(err, message, idx) \
	printf(pixErrThrowStr, __func__, idx, message); \
	err = PIX_ERR_ERROR;\
	goto handle_error_##idx;

#define PIX_ERR_RETURN(err, message) \
	printf(pixErrRetStr, __func__, message); \
	err = PIX_ERR_ERROR;\
	return err;

#define PIX_ERR_CATCH(idx, err, cleanup) \
	if (err != PIX_ERR_SUCCESS) { \
	handle_error_##idx: {\
		cleanup \
	}\
}

#define PIX_ERR_SPECIFY(err, type) \
	if (err != PIX_ERR_SUCCESS && err != PIX_ERR_NOT_SET) {\
		err = type;\
	}
