#pragma once

#include "Utils.h"
#include <G4Event.hh>
#include <G4GeneralParticleSource.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>
#include <G4Types.hh>
#include <G4VUserPrimaryGeneratorAction.hh>
#include <fstream>

namespace SiFi
{

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    explicit PrimaryGeneratorAction(G4GeneralParticleSource* gps) : fSource(gps)
    {
        // G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        // G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
        // if (particle == nullptr) { log->error("Unable to find particle in particle table"); }
        gps->SetParticleDefinition(fParticle);

        // gps->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
        gps->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
        gps->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
    };
    explicit PrimaryGeneratorAction(G4GeneralParticleSource* gps, DataStorage* storage)
        : fSource(gps), fStorage(storage)
    {
        gps->SetParticleDefinition(fParticle);

        // gps->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
        gps->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
        gps->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
        fParticleGun = new G4ParticleGun(10);
        fParticleGun->SetParticleDefinition(fParticle);
        fParticleGun->SetParticleEnergy(4400 * keV);
        fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));
    };

    void GeneratePrimaries(G4Event* event) override
    {
        if (CmdLineOption::GetStringValue("SourcePhaseSpace"))
        {
            G4ThreeVector direction;
            G4ThreeVector position;
            std::ifstream infile(CmdLineOption::GetStringValue("SourcePhaseSpace"));
            double energy, posX, posY, posZ,dirX, dirY, dirZ;
            int id, i=0;

            Float_t sPosX = 0.0;
            Float_t sPosY = 0.0;
            if (CmdLineOption::GetArraySize("Source") == 2)
            {
                sPosX = CmdLineOption::GetDoubleArrayValue("Source", 1);
                sPosY = CmdLineOption::GetDoubleArrayValue("Source", 2);
            }

            std::string line;

            std::getline(infile, line);
            while (infile >> id >> energy >> posX >> posY >> posZ >>dirX >> dirY >> dirZ)
            {
                // position = G4ThreeVector(posY + sPosX, -posZ + sPosY, posX);
                // direction = G4ThreeVector(dirY, -dirZ, dirX);
                position = G4ThreeVector(-posZ + sPosX, posY + sPosY, posX);
                direction = G4ThreeVector(-dirZ, dirY, dirX);
                // if (((direction.theta() < TMath::Pi() / 2) |
                //      ((2.0 * TMath::Pi() - direction.theta()) < TMath::Pi() / 2)))
                // if (((direction.theta() < TMath::Pi() / 6) |
                //      ((2.0 * TMath::Pi() - direction.theta()) < TMath::Pi() / 6)) &&
                //     position.x() < fStorage->getMetadataNumber("sourceMaxX"))
                // {
                    fParticleGun->SetParticlePosition(position);
                    fParticleGun->SetParticleMomentumDirection(direction);
                    fParticleGun->SetParticleEnergy(1000 * energy * keV);
                    fStorage->registerEventStart(i, position, direction, 1000 * energy * keV);
                    fParticleGun->GeneratePrimaryVertex(event);
                    i++;
                // }
            }
            log->info("Done {} particles", i);
        }
        else
        {
            log->debug("GeneratePrimaries");
            fSource->GeneratePrimaryVertex(event);
            log->debug("GeneratePrimaries end");
        }
    }
    const logger log = createLogger("PrimaryGeneratorAction");

private:
    G4GeneralParticleSource* fSource;
    G4ParticleGun* fParticleGun;
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* fParticle = particleTable->FindParticle("gamma");
    DataStorage* fStorage = nullptr;};

} // namespace SiFi
