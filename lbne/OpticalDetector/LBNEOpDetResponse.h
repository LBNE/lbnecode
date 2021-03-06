////////////////////////////////////////////////////////////////////////
// \file LBNEOpDetResponse.h
//
// \brief service containing information about the response of optical detectors in LBNE
//
// \author ahimmel@phy.duke.edu
//
////////////////////////////////////////////////////////////////////////

#ifndef LBNE_OPDET_RESPONSE_H
#define LBNE_OPDET_RESPONSE_H

// LArSoft includes
#include "Simulation/SimPhotons.h"
#include "OpticalDetector/OpDetResponseInterface.h"



namespace opdet
{
    class LBNEOpDetResponse : public opdet::OpDetResponseInterface {
    public:

        LBNEOpDetResponse(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
        ~LBNEOpDetResponse() throw();



    private:

        virtual void doReconfigure(fhicl::ParameterSet const& p);

        virtual int  doNOpChannels() const;
        virtual bool doDetected(int OpChannel, const sim::OnePhoton& Phot, int &newOpChannel) const;
        virtual bool doDetectedLite(int OpChannel, int &newOpChannel) const;

        float fQE;                     // Quantum efficiency of tube
        
        float fWavelengthCutLow;       // Sensitive wavelength range 
        float fWavelengthCutHigh;      // 
        
        bool fLightGuideAttenuation;   // Flag to turn on position-dependent sensitivity

        std::string fChannelConversion;
        bool fFullSimChannelConvert;   // Flag to conver detector->electronics channels in full optical sim
        bool fFastSimChannelConvert;   // Flag to conver detector->electronics channels in fast optical sim

        int fLongAxis;                 // 0 = x, 1 = y, 2 = z

    }; // class LBNEOpDetResponse

    
} //namespace opdet


DECLARE_ART_SERVICE_INTERFACE_IMPL(opdet::LBNEOpDetResponse, opdet::OpDetResponseInterface, LEGACY)

#endif //OPDET_RESPONSE_LBNE_H
