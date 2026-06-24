# MPMCQueue Extension

Status:
draft_v1

Purpose:
Provide bounded lock-free
multiple-producer
multiple-consumer queue.

Primary Consumer:
Layer2 JobDispatch.

Design Goal:

- no heap allocation
- bounded capacity
- deterministic latency
- cache-friendly
- lock-free
