# Horror Castle v1.3 — The Nervous System

The Nervous System is Horror Castle's abstraction boundary for cross-creature physical ecology. Creatures never reach into one another's private DSP state. They publish normalized physical meaning and consume routed physical meaning.

## Public physical signals

Every participating creature may publish six normalized signals in the range 0..1:

- ENERGY — stored or radiated physical energy.
- PRESSURE — pressure, potential, tension, or analogous stored stress.
- MOTION — normalized movement/activity.
- INSTABILITY — proximity to nonlinear transition, discharge, collapse, or chaotic behavior.
- EVENT — a short bounded event pulse. EVENT is never route-smoothed.
- FIELD — a persistent shared/environmental field quantity.

## Routing Matrix

`CreatureRoutingMatrix` is fixed-capacity and allocation-free. It supports up to 16 creature endpoints and 32 simultaneous routes. Each route contains a source creature, source signal, destination creature, destination signal, bipolar amount, smoothing coefficient, and enable state.

Routing uses immutable source snapshots and a fresh destination inbox. A destination's newly routed state is therefore never read again during the same routing frame. This creates a deliberate one-frame causal boundary and prevents recursive zero-delay ecology cycles.

Continuous signals can be smoothed per route. EVENT bypasses route smoothing so a collapse, arc, strike, or discharge remains an event rather than becoming an artificial DC tail. All route amounts and destination values are bounded and non-finite values are sanitized.

## First routed ecology — fluid family

VORTEX publishes pressure, flow-derived motion, instability, collapse EVENT, field, and energy from the actual fluid simulation.

The first route is:

`VORTEX EVENT -> SIREN EVENT inbox -> SIREN plenum disturbance`

The routed collapse pulse perturbs SIREN before sound generation: plenum pressure, field motion, jet operating speed, edge frequency, displacement, and switching behavior can all change. The fluid regression compares routed and disconnected renders and requires an audible difference while remaining finite and bounded.

## Second routed ecology — electromagnetic family

POLTERGEIST publishes charge energy, electrical potential, charging motion, discharge instability, arc EVENT, and field state from its charged-plate simulation.

Two routes can feed AURORA's FIELD inbox:

`POLTERGEIST INSTABILITY -> AURORA FIELD`

`POLTERGEIST EVENT -> AURORA FIELD`

AURORA consumes that routed field before ring charge redistribution and frequency bending. The disturbance therefore changes the ring-field physical regime rather than applying output modulation. AURORA republishes its resulting field, motion, pressure, instability, and energy state for future ecology routes.

## Regression contract

The Nervous System gate requires normalized bounds, finite safety, bipolar route depth, route smoothing, EVENT pulse semantics, invalid-endpoint rejection, fixed-capacity storage, snapshot cycle safety, disabled-route disconnection, audible routed ecology, and extreme-setting boundedness.

Research prototypes remain unassigned to new public generator indices until their productionization gate is explicitly completed. The Nervous System does not alter historical generator identity or preset/state meaning.
