#include "EventAction.h"
#include <Geant4/G4Event.hh>
#include <Geant4/G4PrimaryParticle.hh>
#include <Geant4/G4PrimaryVertex.hh>

namespace SiFi {

void EventAction::BeginOfEventAction(const G4Event* event) {
    auto vertex = event->GetPrimaryVertex();
    auto position = vertex->GetPosition();
    auto direction = vertex->GetPrimary()->GetMomentumDirection();
    eventData.sourcePosition = TVector3(position.x(), position.y(), position.z());
    eventData.sourceDirection = TVector3(direction.x(), direction.y(), direction.z());
}

void EventAction::EndOfEventAction(const G4Event* event) {}

} // namespace SiFi
