/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_common/m2m_base_inc/m2mb_os_types.h

  @brief
    types used across OS (semaphore, mutex...)

  @details
    types used across OS (semaphore, mutex...). E.g. common OS errors

  @notes
    Dependencies:

  @author
    Alessandro Papagno

  @date
    07/03/2017
*/

#ifndef M2MB_OS_TYPE_H
#define M2MB_OS_TYPE_H

/* Include ======================================================================================*/


/* Global declarations ==========================================================================*/

#define M2MB_SEL_ENUM_TASK_START   0x1000
#define M2MB_SEL_ENUM_MTX_START    0x2000
#define M2MB_SEL_ENUM_SEM_START    0x4000
#define M2MB_SEL_ENUM_Q_START      0x8000
#define M2MB_SEL_ENUM_TMR_START    0x10000

#define M2MB_SEL_START(item)   M2MB_SEL_ENUM_##item##_START


#define M2MB_OS_WAIT_FOREVER   0xFFFFFFFF
#define M2MB_OS_NO_WAIT        0x0

/* Global typedefs ==============================================================================*/

typedef enum M2MB_OS_RESULT
{
  M2MB_OS_SUCCESS                        = 0,
  M2MB_OS_DELETED,                       /* = 1   (0x01) */
  M2MB_OS_POOL_ERROR,                    /* = 2   (0x02) */
  M2MB_OS_PTR_ERROR,                     /* = 3   (0x03) */
  M2MB_OS_WAIT_ERROR,                    /* = 4   (0x04) */
  M2MB_OS_SIZE_ERROR,                    /* = 5   (0x05) */
  M2MB_OS_EVENT_ERROR,                   /* = 6   (0x06) */
  M2MB_OS_NO_EVENTS,                     /* = 7   (0x07) */
  M2MB_OS_OPTION_ERROR,                  /* = 8   (0x08) */
  M2MB_OS_QUEUE_ERROR,                   /* = 9   (0x09) */
  M2MB_OS_QUEUE_EMPTY,                   /* = 10  (0x0A) */
  M2MB_OS_QUEUE_FULL,                    /* = 11  (0x0B) */
  M2MB_OS_SEMAPHORE_ERROR,               /* = 12  (0x0C) */
  M2MB_OS_NO_INSTANCE,                   /* = 13  (0x0D) */
  M2MB_OS_TASK_ERROR,                    /* = 14  (0x0E) */
  M2MB_OS_PRIORITY_ERROR,                /* = 15  (0x0F) */
  M2MB_OS_NO_MEMORY,                     /* = 16  (0x10) */
  M2MB_OS_DELETE_ERROR,                  /* = 17  (0x11) */
  M2MB_OS_RESUME_ERROR,                  /* = 18  (0x12) */
  M2MB_OS_CALLER_ERROR,                  /* = 19  (0x13) */
  M2MB_OS_SUSPEND_ERROR,                 /* = 20  (0x14) */
  M2MB_OS_TIMER_ERROR,                   /* = 21  (0x15) */
  M2MB_OS_TICK_ERROR,                    /* = 22  (0x16) */
  M2MB_OS_ACTIVATE_ERROR,                /* = 23  (0x17) */
  M2MB_OS_THRESH_ERROR,                  /* = 24  (0x18) */
  M2MB_OS_SUSPEND_LIFTED,                /* = 25  (0x19) */
  M2MB_OS_WAIT_ABORTED,                  /* = 26  (0x1A) */
  M2MB_OS_WAIT_ABORT_ERROR,              /* = 27  (0x1B) */
  M2MB_OS_MUTEX_ERROR,                   /* = 28  (0x1C) */
  M2MB_OS_NOT_AVAILABLE,                 /* = 29  (0x1D) */
  M2MB_OS_NOT_OWNED,                     /* = 30  (0x1E) */
  M2MB_OS_INHERIT_ERROR,                 /* = 31  (0x1F) */
  M2MB_OS_NOT_DONE,                      /* = 32  (0x20) */
  M2MB_OS_CEILING_EXCEEDED,              /* = 33  (0x21) */
  M2MB_OS_INVALID_CEILING,               /* = 34  (0x22) */

  /* --- */
  M2MB_OS_START_ERROR                  =  48, /* = 48  (0x30) */
  M2MB_OS_TICK_ERROR_TIMER_RUN,
  M2MB_OS_PERIOD_ERROR_TIMER_RUN,
  M2MB_OS_CB_ERROR_TIMER_RUN,
  M2MB_OS_ARG_ERROR_TIMER_RUN,
  M2MB_OS_INVALID_MS2TICKS,
  M2MB_OS_ERR_NOT_USR_ENVIRONMENT,
  M2MB_OS_ERR_INTERNAL_SLEEP,
  M2MB_OS_POOL_MISMATCH_TYPE_VAL,        /* byte with block parameters ... */
  M2MB_ERRNO_NOT_SUPPORTED             = 150,

  /* APPLICATION ERROR SECTION */
  M2MB_OS_APP_GENERIC_ERROR            = 200,
  M2MB_OS_APP_GENERIC_TASK_LIST_INS,

  M2MB_OS_APP_ALIGNEMENT_ERROR         = 240,
  M2MB_OS_APP_ALREADY_LOADED,
  M2MB_OS_APP_INVALID,
  M2MB_OS_APP_INVALID_PROPERTIES,
  M2MB_OS_APP_INVALID_MEMORY,
  M2MB_OS_APP_INVALID_CALLBACK,
  /* ... */
  M2MB_OS_INVALID_ERRNO_REF            = 253,
  M2MB_OS_INVALID_ARG                  = 254,
  M2MB_OS_FEATURE_NOT_SUPPORTED        = 255,

  /* --- */
  M2MB_OS_END,
  ENUM_TO_INT( M2MB_OS_RESULT_E )
} M2MB_OS_RESULT_E;



/* Global functions =============================================================================*/


/* Global define ================================================================================*/











#endif /* M2MB_OS_TYPE_H */
