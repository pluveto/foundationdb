/*
 * networksender.actor.h
 *
 * This source file is part of the FoundationDB open source project
 *
 * Copyright 2013-2018 Apple Inc. and the FoundationDB project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// When actually compiled (NO_INTELLISENSE), include the generated version of this file.  In intellisense use the source
// version.
#if defined(NO_INTELLISENSE) && !defined(FDBRPC_NETWORKSENDER_ACTOR_G_H)
#define FDBRPC_NETWORKSENDER_ACTOR_G_H
#include "fdbrpc/networksender.actor.g.h"
#elif !defined(RPCNETWORKSENDER_ACTOR_H)
#define RPCNETWORKSENDER_ACTOR_H

#include "fdbrpc/FlowTransport.h"
#include "flow/flow.h"
#include "flow/actorcompiler.h" // This must be the last #include.

// This actor is used by FlowTransport to serialize the response to a ReplyPromise across the network
ACTOR template <class T>
void networkSender(Future<T> input, Endpoint endpoint) {
	try {
		T value = wait(input);
		FlowTransport::transport().sendUnreliable(SerializeSource<ErrorOr<EnsureTable<T>>>(value), endpoint, false);
	} catch (Error& err) {
		// if (err.code() == error_code_broken_promise) return;
		if (err.code() == error_code_never_reply) {
			return;
		}
		ASSERT(err.code() != error_code_actor_cancelled);
		FlowTransport::transport().sendUnreliable(SerializeSource<ErrorOr<EnsureTable<T>>>(err), endpoint, false);
	}
}
#include "flow/unactorcompiler.h"

#endif
