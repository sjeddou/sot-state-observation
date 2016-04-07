/*
 *  Copyright 2014 CNRS
 *
 *  Alexis MIFSUD
 */

#ifndef SOT_DYNAMIC_GRAPH_IMU_MODEL_BASE_FLEX_ESTIMATION_HH
#define SOT_DYNAMIC_GRAPH_IMU_MODEL_BASE_FLEX_ESTIMATION_HH

#define SOT_STATE_OBSERVATION_CHECK_UNIQUENESS_IN_TIME

#include <dynamic-graph/entity.h>
#include <dynamic-graph/signal-ptr.h>
#include <dynamic-graph/signal-time-dependent.h>
#include <dynamic-graph/linear-algebra.h>

#include <sot/core/matrix-homogeneous.hh>

#include <state-observation/tools/miscellaneous-algorithms.hpp>
#include <state-observation/flexibility-estimation/model-base-ekf-flex-estimator-imu.hpp>

#include <sot-state-observation/tools/definitions.hh>

namespace sotStateObservation
{
        /**
           \brief
        */
        class DGIMUModelBaseFlexEstimation :
            public dynamicgraph::Entity,
            private boost::noncopyable //
        {
        public:
            /**
            \brief Constructor by name
            */
            DGIMUModelBaseFlexEstimation(const std::string& inName);

            ~DGIMUModelBaseFlexEstimation();



            /// Each entity should provide the name of the class it belongs to
            virtual const std::string& getClassName (void) const
            {
                return CLASS_NAME;
            }

            /// Header documentation of the python class
            virtual std::string getDocString () const
            {
                return
                    "A state observer which takes an IMU and gives the flexibility state\n";
            }

            void setComBiasGuess (const ::dynamicgraph::Vector & x)
            {
                setComBiasGuess_=true;
                comBiasGuess_=convertVector<stateObservation::Vector>(x);
            }
			
            /// Enable or disable the estimation
            void setOn(const bool & b)
            {
                bool a;
                a=b;
                estimator_.setOn(a);
                //std::cout << "\n\n\n\n Estimator is set to " << b << "\n\n\n" << std::endl;
            }

            void setFlexibilityGuess (const ::dynamicgraph::Vector & xh0)
            {
                setFlexibilityGuess_=true;
                flexibilityGuess_=convertVector<stateObservation::Vector>(xh0);
            }

            void setFlexibilityCovariance (const ::dynamicgraph::Matrix & p)
            {
                setFlexibilityCovariance_=true;
                flexibilityCovariance_=convertMatrix<stateObservation::Matrix>(p);
            }

            ::dynamicgraph::Matrix  getFlexibilityCovariance () const
            {
                return convertMatrix<dynamicgraph::Matrix>(estimator_.getFlexibilityCovariance());
            }

            void setSamplingPeriod(const double & dt)
            {
                setDt_=true;
                dt_=dt;
            }

            void setContactModel(const unsigned & nb)
            {
                setContactModel_=true;
                contactModel_=nb;
            }

            void setProcessNoiseCovariance(const ::dynamicgraph::Matrix & q)
            {
                Q_=convertMatrix<stateObservation::Matrix>(q);
                recomputeQ_=true;
            }

            ::dynamicgraph::Matrix getProcessNoiseCovariance() const
            {
                return convertMatrix<dynamicgraph::Matrix>( estimator_.getProcessNoiseCovariance());
            }

            void setMeasurementNoiseCovariance(const ::dynamicgraph::Matrix & r)
            {
                setMeasurementNoiseCovariance_=true;
                measurementNoiseCovariance_=convertMatrix<stateObservation::Matrix>(r);
            }

            ::dynamicgraph::Matrix getMeasurementNoiseCovariance() const
            {
                return convertMatrix<dynamicgraph::Matrix>( estimator_.getMeasurementNoiseCovariance());
            }

            void increment()
            {
                flexibilitySOUT(flexibilitySOUT.getTime()+1);
            }

            int getFlexTime() const
            {
                return flexibilitySOUT.getTime();
            }

            void setKfe(const dynamicgraph::Matrix & m)
            {
                setKfe_=true;
                Kfe_=convertMatrix<stateObservation::Matrix3>(m);
            }

            void setKfv(const dynamicgraph::Matrix & m)
            {
                setKfv_=true;
                Kfv_=convertMatrix<stateObservation::Matrix3>(m);
            }

            void setKte(const dynamicgraph::Matrix & m)
            {
                setKte_=true;
                Kte_=convertMatrix<stateObservation::Matrix3>(m);
            }

            void setKtv(const dynamicgraph::Matrix & m)
            {
                setKtv_=true;
                Ktv_=convertMatrix<stateObservation::Matrix3>(m);
            }


            void setWithForce(const bool & b)
            {
                withForcesMeasurements_=true;
            }

            void setWithComBias(const bool & b)
            {
                withComBias_=b;
            }

            void setForceVariance(const double & d)
            {
                setForceVariance_=true;
                forceVariance_=d;
            }

            void setBias(const dynamicgraph::Vector & bias)
            {
                bias_=convertVector<stateObservation::Vector>(bias).block(0,0,2,1);
            }

            void setRobotMass(const double & m)
            {
                setRobotMass_=true;
                robotMass_=m;
            }

            double getRobotMass() const
            {
                return estimator_.getRobotMass();
            }


            void setWithAbsolutePosition(const bool & b)
            {
              estimator_.setWithAbsolutePos(b);
            }

            void setAbsolutePosVariance(const double & d)
            {
                estimator_.setAbsolutePosVariance(d);
            }



            /**
            \name Parameters
            @{
            */
        protected:
            /*
            \brief Class name
            */
            static const std::string CLASS_NAME;

        private:
            /**
            */
            ::dynamicgraph::Vector& computeState
                        (::dynamicgraph::Vector & state, const int& inTime);

            ::dynamicgraph::Vector& computeFlexibility
                          (dynamicgraph::Vector & flexibility, const int& inTime);

            ::dynamicgraph::Vector& computeFlexPosition
                        (::dynamicgraph::Vector & flexibilityPosition, const int& inTime);

            ::dynamicgraph::Vector& computeFlexVelocity
                        (::dynamicgraph::Vector & flexibilityVelocity, const int& inTime);

            ::dynamicgraph::Vector& computeContactsForces
                        (::dynamicgraph::Vector & contactsForces, const int& inTime);

            ::dynamicgraph::Vector& computeFlexThetaU
                        (::dynamicgraph::Vector & flexibilityThetaU, const int& inTime);

            ::dynamicgraph::Vector& computeFlexOmega
                        (::dynamicgraph::Vector & flexibilityOmega, const int& inTime);

            ::dynamicgraph::Vector& computeContactsTorques
                        (::dynamicgraph::Vector & contactsTorques, const int& inTime);



            ::dynamicgraph::sot::MatrixHomogeneous& computeFlexTransformationMatrix
                        (::dynamicgraph::sot::MatrixHomogeneous & flexibilityTransformationMatrix,
                                                            const int& inTime);

            ::dynamicgraph::Vector& computeFlexPoseThetaU
                        (::dynamicgraph::Vector & flexibilityThetaU, const int& inTime);

            ::dynamicgraph::Vector& computeFlexVelocityVector
                        (::dynamicgraph::Vector & flexVelocityVector, const int& inTime);

            ::dynamicgraph::Vector& computeContactsForcesAndTorques
                        (::dynamicgraph::Vector & contactsForcesAndTorques, const int& inTime);



            ::dynamicgraph::Vector& computeFlexInverse
                        (::dynamicgraph::Vector & flexInverse, const int& inTime);

            ::dynamicgraph::sot::MatrixHomogeneous& computeFlexMatrixInverse
                        (::dynamicgraph::sot::MatrixHomogeneous & flexMatrixInverse,
                                                            const int& inTime);

            ::dynamicgraph::Vector& computeFlexInversePoseThetaU
                        (::dynamicgraph::Vector & flexInversePoseThetaU, const int& inTime);

            ::dynamicgraph::Vector& computeFlexInverseVelocityVector
                        (::dynamicgraph::Vector & flexInverseVelocityVector, const int& inTime);

            ::dynamicgraph::Vector& computeFlexInverseVelocity
                        (::dynamicgraph::Vector & flexInverseVelocity, const int& inTime);

            ::dynamicgraph::Vector& computeFlexInverseOmega
                        (::dynamicgraph::Vector & flexInverseOmega, const int &inTime);

            ::dynamicgraph::Vector& computeComBias
                                (::dynamicgraph::Vector & comBias, const int& inTime);

            ::dynamicgraph::Vector& computeSimulatedSensors
                        (::dynamicgraph::Vector & sensorSignal, const int &inTime);

            ::dynamicgraph::Vector& getForcesAndMoments
                        (::dynamicgraph::Vector & forcesAndMoments, const int &inTime);

            dynamicgraph::Vector & getStateCovariance
                    (::dynamicgraph::Vector & stateCovariance, const int& inTime);

            ::dynamicgraph::Vector& getForcesSupport1
                    (::dynamicgraph::Vector & forcesSupport1, const int& inTime);

            ::dynamicgraph::Vector& getForcesSupport2
                    (::dynamicgraph::Vector & forcesSupport2, const int& inTime);

            ::dynamicgraph::Vector& computeInovation
                        (::dynamicgraph::Vector & inovation, const int &inTime);

            ::dynamicgraph::Vector& computePrediction
                        (::dynamicgraph::Vector & prediction, const int &inTime);


            ::dynamicgraph::Vector& computePredictedSensors
                        (::dynamicgraph::Vector & sensorSignal, const int &inTime);

            double& computeFlexibilityComputationTime
                        (double& flexibilityComputationTime, const int &inTime);




            /**
            \brief Measurement of the IMU
            */
            dynamicgraph::SignalPtr < ::dynamicgraph::Vector, int> measurementSIN;


            /**
            \brief Input of the dynamical system
            */
            dynamicgraph::SignalPtr < ::dynamicgraph::Vector, int> inputSIN;

            /**
            \brief Number of the contact points
            */
            dynamicgraph::SignalPtr < unsigned , int> contactsNbrSIN;


            dynamicgraph::SignalTimeDependent
                            < ::dynamicgraph::Vector, int> forcesAndMomentsSOUT;
            dynamicgraph::SignalTimeDependent
                            < ::dynamicgraph::Vector, int> forcesSupport1SOUT;
            dynamicgraph::SignalTimeDependent
                            < ::dynamicgraph::Vector, int> forcesSupport2SOUT;

            /**
            \brief Estimation of the state
            */
            dynamicgraph::Signal < ::dynamicgraph::Vector, int> stateSOUT;


            /**
            \brief Estimation of the flexibility
            */
            dynamicgraph::Signal < ::dynamicgraph::Vector, int> flexibilitySOUT;

            /**
            \brief Different parts of the vector of the flexibility estimation vector
            */
            dynamicgraph::SignalTimeDependent
                            < ::dynamicgraph::Vector, int> flexPositionSOUT;

            dynamicgraph::SignalTimeDependent
                            < ::dynamicgraph::Vector, int> flexVelocitySOUT;

            dynamicgraph::SignalTimeDependent
                            < ::dynamicgraph::Vector, int> contactsForcesSOUT;

            dynamicgraph::SignalTimeDependent
                            < ::dynamicgraph::Vector, int> flexThetaUSOUT;

            dynamicgraph::SignalTimeDependent
                            < ::dynamicgraph::Vector, int> flexOmegaSOUT;

            dynamicgraph::SignalTimeDependent
                            < ::dynamicgraph::Vector, int> contactsTorquesSOUT;

            /**
            \brief Transformed parts of the flexibility state vector
            */
            dynamicgraph::SignalTimeDependent
                            < ::dynamicgraph::Vector, int> flexPoseThetaUSOUT;

            dynamicgraph::SignalTimeDependent
                            < ::dynamicgraph::sot::MatrixHomogeneous, int>
                                                flexTransformationMatrixSOUT;
            dynamicgraph::SignalTimeDependent
                            < ::dynamicgraph::Vector, int> flexVelocityVectorSOUT;

            dynamicgraph::SignalTimeDependent
                            < ::dynamicgraph::Vector, int> contactsForcesAndTorquesSOUT;


            /**
            \brief Various parts of the inverse flexibility
            */
            dynamicgraph::SignalTimeDependent
                                < ::dynamicgraph::Vector, int> flexInverseSOUT;
            dynamicgraph::SignalTimeDependent
                                < ::dynamicgraph::sot::MatrixHomogeneous, int>
                                                        flexMatrixInverseSOUT;
            dynamicgraph::SignalTimeDependent
                                < ::dynamicgraph::Vector,int> flexInversePoseThetaUSOUT;
            dynamicgraph::SignalTimeDependent
                                < ::dynamicgraph::Vector,int> flexInverseVelocityVectorSOUT;
            dynamicgraph::SignalTimeDependent
                                < ::dynamicgraph::Vector,int> flexInverseVelocitySOUT;
            dynamicgraph::SignalTimeDependent
                                < ::dynamicgraph::Vector,int> flexInverseOmegaSOUT;

            dynamicgraph::SignalTimeDependent
                                < ::dynamicgraph::Vector,int> comBiasSOUT;

            /**
            \brief A simulation of the sensors' signals
            */
            dynamicgraph::SignalTimeDependent
                                < ::dynamicgraph::Vector,int> simulatedSensorsSOUT;

            dynamicgraph::SignalTimeDependent
                                < ::dynamicgraph::Vector,int> predictedSensorsSOUT;

            /**
            \brief Evaluation of the time of flexibility computation
            */
            dynamicgraph::SignalTimeDependent
                                < double,int> flexibilityComputationTimeSOUT;


            /**
            \brief A simulation of the sensors' signals
            */
            dynamicgraph::SignalTimeDependent
                                < ::dynamicgraph::Vector,int> inovationSOUT;


            dynamicgraph::SignalTimeDependent
                                < ::dynamicgraph::Vector,int> predictionSOUT;

            dynamicgraph::SignalTimeDependent
                                < ::dynamicgraph::Vector,int> stateCovarianceSOUT;


            stateObservation::flexibilityEstimation::ModelBaseEKFFlexEstimatorIMU estimator_;


            ///Sizes of the states for the state, the measurement, and the input vector
            unsigned stateSize;
            static const unsigned measurementSize=12;
            static const unsigned inputSizeBase=42;
            unsigned inputSize_;
            unsigned contactNumber_;

            bool withComBias_;
            bool withForcesMeasurements_;

            stateObservation::Vector bias_;
            int currentTime_;

            stateObservation::Vector flexibilityGuess_;
            bool setFlexibilityGuess_;

            stateObservation::Matrix flexibilityCovariance_;
            bool setFlexibilityCovariance_;

            stateObservation::Vector comBiasGuess_;
            bool setComBiasGuess_;

            stateObservation::Matrix Q_;
            bool recomputeQ_;

            double dt_;
            bool setDt_;

            bool setContactModel_;
            unsigned contactModel_;

            bool setMeasurementNoiseCovariance_;
            stateObservation::Matrix measurementNoiseCovariance_;

            bool setKfe_;
            stateObservation::Matrix3 Kfe_;

            bool setKfv_;
            stateObservation::Matrix3 Kfv_;

            bool setKte_;
            stateObservation::Matrix3 Kte_;

            bool setKtv_;
            stateObservation::Matrix3 Ktv_;

            bool setForceVariance_;
            double forceVariance_;

            bool setRobotMass_;
            double robotMass_;

        };

} // namespace sotStateObservation

#endif //SOT_DYNAMIC_GRAPH_IMU_MODEL_BASE_FLEX_ESTIMATION_HH
