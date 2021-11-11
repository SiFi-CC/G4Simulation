#pragma once

#include "DataStorage.h"
#include "Utils.h"
#include <G4UserEventAction.hh>

namespace SiFi
{

class EventAction : public G4UserEventAction
{
public:
    EventAction(DataStorage* storage) : fStorage(storage){};
    void BeginOfEventAction(const G4Event* evt) override;
    void EndOfEventAction(const G4Event* evt) override;

    const logger log = createLogger("EventAction");

private:
    DataStorage* fStorage = nullptr;
};

} // namespace SiFi
