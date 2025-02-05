/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSemReqRelease
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <string.h>

#include "tr-mtx-surrender.h"
#include "tr-sem-surrender.h"
#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSemReqRelease spec:/rtems/sem/req/release
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSemReqRelease_Pre_Class_Counting,
  RtemsSemReqRelease_Pre_Class_Simple,
  RtemsSemReqRelease_Pre_Class_Binary,
  RtemsSemReqRelease_Pre_Class_PrioCeiling,
  RtemsSemReqRelease_Pre_Class_PrioInherit,
  RtemsSemReqRelease_Pre_Class_MrsP,
  RtemsSemReqRelease_Pre_Class_NA
} RtemsSemReqRelease_Pre_Class;

typedef enum {
  RtemsSemReqRelease_Pre_Discipline_FIFO,
  RtemsSemReqRelease_Pre_Discipline_Priority,
  RtemsSemReqRelease_Pre_Discipline_NA
} RtemsSemReqRelease_Pre_Discipline;

typedef enum {
  RtemsSemReqRelease_Pre_Id_Valid,
  RtemsSemReqRelease_Pre_Id_Invalid,
  RtemsSemReqRelease_Pre_Id_NA
} RtemsSemReqRelease_Pre_Id;

typedef enum {
  RtemsSemReqRelease_Post_Action_InvId,
  RtemsSemReqRelease_Post_Action_BinarySurrender,
  RtemsSemReqRelease_Post_Action_CountingSurrender,
  RtemsSemReqRelease_Post_Action_MtxSurrender,
  RtemsSemReqRelease_Post_Action_InheritMtxSurrender,
  RtemsSemReqRelease_Post_Action_CeilingMtxSurrender,
  RtemsSemReqRelease_Post_Action_MrsPMtxSurrender,
  RtemsSemReqRelease_Post_Action_NA
} RtemsSemReqRelease_Post_Action;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Class_NA : 1;
  uint8_t Pre_Discipline_NA : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Post_Action : 3;
} RtemsSemReqRelease_Entry;

/**
 * @brief Test context for spec:/rtems/sem/req/release test case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  union {
    TQContext tq_ctx;
    TQMtxContext tq_mtx_ctx;
    TQSemContext tq_sem_ctx;
  };

  /**
   * @brief This member specifies if the attribute set of the semaphore.
   */
  rtems_attribute attribute_set;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

    /**
     * @brief If this member is true, then the test action loop is executed.
     */
    bool in_action_loop;

    /**
     * @brief This member contains the next transition map index.
     */
    size_t index;

    /**
     * @brief This member contains the current transition map entry.
     */
    RtemsSemReqRelease_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSemReqRelease_Context;

static RtemsSemReqRelease_Context
  RtemsSemReqRelease_Instance;

static const char * const RtemsSemReqRelease_PreDesc_Class[] = {
  "Counting",
  "Simple",
  "Binary",
  "PrioCeiling",
  "PrioInherit",
  "MrsP",
  "NA"
};

static const char * const RtemsSemReqRelease_PreDesc_Discipline[] = {
  "FIFO",
  "Priority",
  "NA"
};

static const char * const RtemsSemReqRelease_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const * const RtemsSemReqRelease_PreDesc[] = {
  RtemsSemReqRelease_PreDesc_Class,
  RtemsSemReqRelease_PreDesc_Discipline,
  RtemsSemReqRelease_PreDesc_Id,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

typedef RtemsSemReqRelease_Context Context;

static void RtemsSemReqRelease_Pre_Class_Prepare(
  RtemsSemReqRelease_Context  *ctx,
  RtemsSemReqRelease_Pre_Class state
)
{
  switch ( state ) {
    case RtemsSemReqRelease_Pre_Class_Counting: {
      /*
       * While the semaphore object is a counting semaphore.
       */
      ctx->attribute_set |= RTEMS_COUNTING_SEMAPHORE;
      break;
    }

    case RtemsSemReqRelease_Pre_Class_Simple: {
      /*
       * While the semaphore object is a simple binary semaphore.
       */
      ctx->attribute_set |= RTEMS_SIMPLE_BINARY_SEMAPHORE;
      break;
    }

    case RtemsSemReqRelease_Pre_Class_Binary: {
      /*
       * While the semaphore object is a binary semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE;
      break;
    }

    case RtemsSemReqRelease_Pre_Class_PrioCeiling: {
      /*
       * While the semaphore object is a priority ceiling semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY_CEILING;
      break;
    }

    case RtemsSemReqRelease_Pre_Class_PrioInherit: {
      /*
       * While the semaphore object is a priority inheritance semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY;
      break;
    }

    case RtemsSemReqRelease_Pre_Class_MrsP: {
      /*
       * While the semaphore object is a MrsP semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE |
        RTEMS_MULTIPROCESSOR_RESOURCE_SHARING;
      break;
    }

    case RtemsSemReqRelease_Pre_Class_NA:
      break;
  }
}

static void RtemsSemReqRelease_Pre_Discipline_Prepare(
  RtemsSemReqRelease_Context       *ctx,
  RtemsSemReqRelease_Pre_Discipline state
)
{
  switch ( state ) {
    case RtemsSemReqRelease_Pre_Discipline_FIFO: {
      /*
       * While the semaphore uses the FIFO task wait queue discipline.
       */
      ctx->attribute_set |= RTEMS_FIFO;
      ctx->tq_ctx.discipline = TQ_FIFO;
      break;
    }

    case RtemsSemReqRelease_Pre_Discipline_Priority: {
      /*
       * While the semaphore uses the priority task wait queue discipline.
       */
      ctx->attribute_set |= RTEMS_PRIORITY;
      ctx->tq_ctx.discipline = TQ_PRIORITY;
      break;
    }

    case RtemsSemReqRelease_Pre_Discipline_NA:
      break;
  }
}

static void RtemsSemReqRelease_Pre_Id_Prepare(
  RtemsSemReqRelease_Context *ctx,
  RtemsSemReqRelease_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsSemReqRelease_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is associated with the semaphore.
       */
      /* Nothing to prepare */
      break;
    }

    case RtemsSemReqRelease_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is not associated with a semaphore.
       */
      /* Nothing to prepare */
      break;
    }

    case RtemsSemReqRelease_Pre_Id_NA:
      break;
  }
}

static void RtemsSemReqRelease_Post_Action_Check(
  RtemsSemReqRelease_Context    *ctx,
  RtemsSemReqRelease_Post_Action state
)
{
  rtems_status_code sc;

  switch ( state ) {
    case RtemsSemReqRelease_Post_Action_InvId: {
      /*
       * The return status of rtems_semaphore_release() shall be
       * RTEMS_INVALID_ID.
       */
      sc = rtems_semaphore_release( 0xffffffff );
      T_rsc( sc, RTEMS_INVALID_ID );
      break;
    }

    case RtemsSemReqRelease_Post_Action_BinarySurrender: {
      /*
       * The calling task shall surrender the binary semaphore as specified by
       * /score/sem/req/surrender.
       */
      ctx->tq_ctx.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      ctx->tq_ctx.get_owner = NULL;
      ctx->tq_sem_ctx.variant = TQ_SEM_BINARY;
      ctx->tq_sem_ctx.get_count = TQSemGetCountClassic;
      ctx->tq_sem_ctx.set_count = TQSemSetCountClassic;
      ScoreSemReqSurrender_Run( &ctx->tq_sem_ctx );
      break;
    }

    case RtemsSemReqRelease_Post_Action_CountingSurrender: {
      /*
       * The calling task shall surrender the counting semaphore as specified
       * by /score/sem/req/surrender.
       */
      ctx->tq_ctx.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      ctx->tq_ctx.get_owner = NULL;
      ctx->tq_sem_ctx.variant = TQ_SEM_COUNTING;
      ctx->tq_sem_ctx.get_count = TQSemGetCountClassic;
      ctx->tq_sem_ctx.set_count = TQSemSetCountClassic;
      ScoreSemReqSurrender_Run( &ctx->tq_sem_ctx );
      break;
    }

    case RtemsSemReqRelease_Post_Action_MtxSurrender: {
      /*
       * The calling task shall surrender the mutex as specified by
       * /score/mtx/req/surrender where an enqueue blocks, a recursive seize is
       * allowed, the owner is checked, and no locking protocol is used.
       */
      ctx->tq_ctx.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      ctx->tq_ctx.get_owner = TQGetOwnerClassicSem;
      ctx->tq_mtx_ctx.protocol = TQ_MTX_NO_PROTOCOL;
      ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
      ctx->tq_mtx_ctx.owner_check = TQ_MTX_CHECKS_OWNER;
      ctx->tq_mtx_ctx.priority_ceiling = PRIO_INVALID;
      ScoreMtxReqSurrender_Run( &ctx->tq_mtx_ctx );
      break;
    }

    case RtemsSemReqRelease_Post_Action_InheritMtxSurrender: {
      /*
       * The calling task shall surrender the mutex as specified by
       * /score/mtx/req/surrender where an enqueue blocks, a recursive seize is
       * allowed, the owner is checked, and a priority inheritance protocol is
       * used.
       */
      ctx->tq_ctx.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      ctx->tq_ctx.get_owner = TQGetOwnerClassicSem;
      ctx->tq_mtx_ctx.protocol = TQ_MTX_PRIORITY_INHERIT;
      ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
      ctx->tq_mtx_ctx.owner_check = TQ_MTX_CHECKS_OWNER;
      ctx->tq_mtx_ctx.priority_ceiling = PRIO_INVALID;
      ScoreMtxReqSurrender_Run( &ctx->tq_mtx_ctx );
      break;
    }

    case RtemsSemReqRelease_Post_Action_CeilingMtxSurrender: {
      /*
       * The calling task shall surrender the mutex as specified by
       * /score/mtx/req/surrender where an enqueue blocks, a recursive seize is
       * allowed, the owner is checked, and a priority ceiling is used.
       */
      ctx->tq_ctx.enqueue_variant = TQ_ENQUEUE_BLOCKS;
      ctx->tq_ctx.get_owner = TQGetOwnerClassicSem;
      ctx->tq_mtx_ctx.protocol = TQ_MTX_PRIORITY_CEILING;
      ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
      ctx->tq_mtx_ctx.owner_check = TQ_MTX_CHECKS_OWNER;
      ctx->tq_mtx_ctx.priority_ceiling = PRIO_VERY_HIGH;
      ScoreMtxReqSurrender_Run( &ctx->tq_mtx_ctx );
      break;
    }

    case RtemsSemReqRelease_Post_Action_MrsPMtxSurrender: {
      /*
       * The calling task shall surrender the mutex as specified by
       * /score/mtx/req/surrender where an enqueue is sticky, a recursive seize
       * returns an error status, the owner is checked, and a priority ceiling
       * is used.
       */
      ctx->tq_ctx.enqueue_variant = TQ_ENQUEUE_STICKY;
      ctx->tq_ctx.get_owner = TQGetOwnerClassicSem;
      ctx->tq_mtx_ctx.protocol = TQ_MTX_MRSP;
      ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_DEADLOCK;
      ctx->tq_mtx_ctx.owner_check = TQ_MTX_CHECKS_OWNER;
      ctx->tq_mtx_ctx.priority_ceiling = PRIO_VERY_HIGH;
      ScoreMtxReqSurrender_Run( &ctx->tq_mtx_ctx );
      break;
    }

    case RtemsSemReqRelease_Post_Action_NA:
      break;
  }
}

static void RtemsSemReqRelease_Setup( RtemsSemReqRelease_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->tq_ctx.deadlock = TQ_DEADLOCK_STATUS;
  ctx->tq_ctx.enqueue_prepare = TQEnqueuePrepareDefault;
  ctx->tq_ctx.enqueue_done = TQEnqueueDoneDefault;
  ctx->tq_ctx.enqueue = TQEnqueueClassicSem;
  ctx->tq_ctx.surrender = TQSurrenderClassicSem;
  ctx->tq_ctx.convert_status = TQConvertStatusClassic;
  TQInitialize( &ctx->tq_ctx );
}

static void RtemsSemReqRelease_Setup_Wrap( void *arg )
{
  RtemsSemReqRelease_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqRelease_Setup( ctx );
}

static void RtemsSemReqRelease_Teardown( RtemsSemReqRelease_Context *ctx )
{
  TQDestroy( &ctx->tq_ctx );
  RestoreRunnerPriority();
}

static void RtemsSemReqRelease_Teardown_Wrap( void *arg )
{
  RtemsSemReqRelease_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqRelease_Teardown( ctx );
}

static void RtemsSemReqRelease_Prepare( RtemsSemReqRelease_Context *ctx )
{
  ctx->attribute_set = RTEMS_DEFAULT_ATTRIBUTES;
}

static void RtemsSemReqRelease_Action( RtemsSemReqRelease_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_semaphore_create(
    NAME,
    1,
    ctx->attribute_set,
    PRIO_VERY_HIGH,
    &ctx->tq_ctx.thread_queue_id
  );
  T_rsc_success( sc );

  if ( ( ctx->attribute_set & RTEMS_MULTIPROCESSOR_RESOURCE_SHARING ) != 0 ) {
    rtems_task_priority prio;

    sc = rtems_semaphore_set_priority(
      ctx->tq_ctx.thread_queue_id,
      SCHEDULER_B_ID,
      PRIO_VERY_HIGH,
      &prio
    );
    T_rsc_success( sc );
  }
}

static void RtemsSemReqRelease_Cleanup( RtemsSemReqRelease_Context *ctx )
{
  rtems_status_code sc;
  sc = rtems_semaphore_delete( ctx->tq_ctx.thread_queue_id ); T_rsc_success( sc );
}

static const RtemsSemReqRelease_Entry
RtemsSemReqRelease_Entries[] = {
  { 0, 0, 0, 0, RtemsSemReqRelease_Post_Action_InvId },
  { 1, 0, 0, 0, RtemsSemReqRelease_Post_Action_NA },
  { 0, 0, 0, 0, RtemsSemReqRelease_Post_Action_CountingSurrender },
  { 0, 0, 0, 0, RtemsSemReqRelease_Post_Action_BinarySurrender },
  { 0, 0, 0, 0, RtemsSemReqRelease_Post_Action_MtxSurrender },
  { 0, 0, 0, 0, RtemsSemReqRelease_Post_Action_CeilingMtxSurrender },
  { 0, 0, 0, 0, RtemsSemReqRelease_Post_Action_InheritMtxSurrender },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, RtemsSemReqRelease_Post_Action_MrsPMtxSurrender },
#else
  { 1, 0, 0, 0, RtemsSemReqRelease_Post_Action_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, RtemsSemReqRelease_Post_Action_InvId }
#else
  { 1, 0, 0, 0, RtemsSemReqRelease_Post_Action_NA }
#endif
};

static const uint8_t
RtemsSemReqRelease_Map[] = {
  2, 0, 2, 0, 3, 0, 3, 0, 4, 0, 4, 0, 1, 1, 5, 0, 1, 1, 6, 0, 1, 1, 7, 8
};

static size_t RtemsSemReqRelease_Scope( void *arg, char *buf, size_t n )
{
  RtemsSemReqRelease_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsSemReqRelease_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsSemReqRelease_Fixture = {
  .setup = RtemsSemReqRelease_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsSemReqRelease_Teardown_Wrap,
  .scope = RtemsSemReqRelease_Scope,
  .initial_context = &RtemsSemReqRelease_Instance
};

static inline RtemsSemReqRelease_Entry RtemsSemReqRelease_PopEntry(
  RtemsSemReqRelease_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSemReqRelease_Entries[
    RtemsSemReqRelease_Map[ index ]
  ];
}

static void RtemsSemReqRelease_TestVariant( RtemsSemReqRelease_Context *ctx )
{
  RtemsSemReqRelease_Pre_Class_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSemReqRelease_Pre_Discipline_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsSemReqRelease_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsSemReqRelease_Action( ctx );
  RtemsSemReqRelease_Post_Action_Check( ctx, ctx->Map.entry.Post_Action );
}

/**
 * @fn void T_case_body_RtemsSemReqRelease( void )
 */
T_TEST_CASE_FIXTURE( RtemsSemReqRelease, &RtemsSemReqRelease_Fixture )
{
  RtemsSemReqRelease_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsSemReqRelease_Pre_Class_Counting;
    ctx->Map.pcs[ 0 ] < RtemsSemReqRelease_Pre_Class_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsSemReqRelease_Pre_Discipline_FIFO;
      ctx->Map.pcs[ 1 ] < RtemsSemReqRelease_Pre_Discipline_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsSemReqRelease_Pre_Id_Valid;
        ctx->Map.pcs[ 2 ] < RtemsSemReqRelease_Pre_Id_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsSemReqRelease_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        RtemsSemReqRelease_Prepare( ctx );
        RtemsSemReqRelease_TestVariant( ctx );
        RtemsSemReqRelease_Cleanup( ctx );
      }
    }
  }
}

/** @} */
