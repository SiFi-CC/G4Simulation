#include "EventAction.h"
#include <G4Event.hh>
#include <G4PrimaryParticle.hh>
#include <G4PrimaryVertex.hh>

namespace SiFi {

void EventAction::BeginOfEventAction(const G4Event* event) {
    auto vertex = event->GetPrimaryVertex();
    auto position = vertex->GetPosition();
    auto direction = vertex->GetPrimary()->GetMomentumDirection();
    auto energy = vertex->GetPrimary()->GetKineticEnergy();
    fStorage->registerEventStart(event->GetEventID(), position, direction, energy);
}

void EventAction::EndOfEventAction(const G4Event* event) {}

} // namespace SiFi
