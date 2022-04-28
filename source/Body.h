/*
 * Copyright (c) 2014 Matt Hall <mtjhall@alumni.uvic.ca>
 * 
 * This file is part of MoorDyn.  MoorDyn is free software: you can redistribute 
 * it and/or modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 * 
 * MoorDyn is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with MoorDyn.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BODY_H
#define BODY_H

#include "Misc.h"
#include "Connection.hpp"

using namespace std;

class Rod;
namespace moordyn {
	// NOTE: The namespace will be extended to the class below in the future
	class Waves;
}

class Body
{
	
	
	// ENVIRONMENTAL STUFF	
	EnvCond *env;  // pointer to global struct that holds environmental settings
	moordyn::Waves *waves;  // pointer to global Waves object
	
	
	// unique to Body:
	moordyn::Connection* attachedC[30]; 	// pointers to connections attached to this body
	int nAttachedC; 		// quantity of attached connections
	
	Rod* attachedR[30]; 	// pointers to Rods attached to this body
	int nAttachedR; 		// quantity of attached Rods
	
	// arrays to hold relative coordinates of attached Connections and Rods
	double rConnectRel[30][3];
	double r6RodRel   [30][6]; 
	
	
	double body_r6[6]; // constants set at startup from input file
	double body_rCG[3];
	double bodyM;
	double bodyV;
	double bodyI[3];
	double bodyCdA[6];
	double bodyCa[6];
	
	// input file has	Name/ID      X0   Y0   Z0   Xcg   Ycg   Zcg      M      V        IX       IY       IZ     CdA-x,y,z Ca-x,y,z
	
	// degrees of freedom (or states)
	double r6 [6]; 		// body 6dof position [x/y/z] // double* to continuous state or input state or parameter/other-state
	double v6 [6];		// body 6dof velocity[x/y/z]  // double* to continuous state or input state or parameter/other-state
	
	double t; 					// simulation time
	double t0; 					// simulation time current integration was started at (used for BC function)
	double r_ves[6]; 		    // fairlead position for coupled bodies [x/y/z]
	double rd_ves[6];		    // fairlead velocity for coupled bodies [x/y/z]

	double F6net  [6];	// total force and moment vector on node

	double M[6][6]; // total body mass + added mass matrix including all elements
	double M0[6][6]; // starting mass and added mass matrix (6x6) of body without any rod elements in inertital orientation
	
	double OrMat[9]; // orientation matrix of body (rotation matrix that gets it to its current orientation)

	double  U[3];     // wave velocities at body reference point
	double  Ud[3];     // wave accelerations


	ofstream * outfile;

public:

	/** @brief Types of bodies
	 */
	typedef enum {
		/// Is coupled, i.e. is controlled by the user
		COUPLED = -1,
		/// Is free to move, controlled by MoorDyn
		FREE = 0,
		/// Is fixed, either to a location or to another moving entity
		FIXED = 1,
		// Some aliases
		VESSEL = COUPLED,
		ANCHOR = FIXED,
	} types;

	/** @brief Return a string with the name of a type
	 *
	 * This tool is useful mainly for debugging
	 */
	static string TypeName(types t)
	{
		switch(t)
		{
		case COUPLED:
			return "COUPLED";
		case FREE:
			return "FREE";
		case FIXED:
			return "FIXED";
		}
		return "UNKNOWN";
	}

	//
	int number;
	types type;  // <<< N/A ??
		
	void setup(int number_in, types type_in, double r6_in[6], double rCG_in[3], double M_in,
	double V_in, double I_in[3], double CdA_in[3], double Ca_in[3], shared_ptr<ofstream> outfile_pointer);
	
	void addConnectionToBody(moordyn::Connection *theConnection, double coords[3]);
	
	void addRodToBody(Rod *theRod, double endCoords[6]);
	
	void initializeUnfreeBody(const double r_in[6], const double rd_in[6], double time);
	
	void initializeBody( double* X );

	void setEnv(EnvCond *env_in, moordyn::Waves *waves_in);

	void setDependentStates();
	
	void getBodyState(double r_out[6], double rd_out[6]);
		
	void getFnet(double Fnet_out[]) const;
	
	void getM(double M_out[6][6]);
	
	double GetBodyOutput(OutChanProps outChan);
		
	void scaleDrag(double scaler);	
	void setTime(double time);
	
	void initiateStep(const double r_in[6], const double rd_in[6], double time);
	void updateFairlead( const double time);
	
	void setState( const double* X, const double time);
	
	int getStateDeriv( double* Xd);
	
	void doRHS();
	
	void Output(double );
	
	~Body();
	
	#ifdef USEGL
	void drawGL(void);
	#endif
	
};

#endif
