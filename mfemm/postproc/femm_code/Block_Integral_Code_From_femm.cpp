// came from FemmviewDoc.cpp


CComplex CFemmviewDoc::BlockIntegral(int inttype)
{
	int i,k;
	CComplex c,y,z,J,mu1,mu2,B1,B2,H1,H2,F1,F2;
	CComplex A[3],Jn[3],U[3],V[3];
	double a,sig,R;
	double r[3];

	z=0; for(i=0;i<3;i++) U[i]=1.;

	if(inttype==6) z= BlockIntegral(3) + BlockIntegral(4); //total losses
	else for(i=0;i<meshelem.GetSize();i++)
	{
	  if(blocklist[meshelem[i].lbl].IsSelected==TRUE)
	  {

		// compute some useful quantities employed by most integrals...
		J=GetJA(i,Jn,A);
		a=ElmArea(i)*pow(LengthConv[LengthUnits],2.);
		if(ProblemType==1){
			for(k=0;k<3;k++)
				r[k]=meshnode[meshelem[i].p[k]].x*LengthConv[LengthUnits];
			R=(r[0]+r[1]+r[2])/3.;
		}

		// now, compute the desired integral;
		switch(inttype)
		{
			case 0: //  A.J
				for(k=0;k<3;k++) V[k]=Jn[k].Conj();
				if(ProblemType==0)
					y=PlnInt(a,A,V)*Depth;
				else
					y=AxiInt(a,A,V,r);
				z+=y;

				break;

			case 11: // x (or r) direction Lorentz force, SS part.
				B2=meshelem[i].B2;
				y= -(B2.re*J.re + B2.im*J.im);
				if (ProblemType==1) y=0; else y*=Depth;
				if(Frequency!=0) y*=0.5;
				z+=(a*y);
				break;

			case 12: // y (or z) direction Lorentz force, SS part.
				for(k=0;k<3;k++) V[k]=Re(meshelem[i].B1*Jn[k].Conj());
				if(ProblemType==0)
					y=PlnInt(a,U,V)*Depth;
				else
					y=AxiInt(-a,U,V,r);
				if(Frequency!=0) y*=0.5;
				z+=y;

				break;

			case 13: // x (or r) direction Lorentz force, 2x part.
				if((Frequency!=0) && (ProblemType==0))
				{
					B2=meshelem[i].B2;
					y= -(B2.re*J.re - B2.im*J.im) - I*(B2.re*J.im+B2.im*J.re);
					z+=0.5*(a*y*Depth);
				}
				break;

			case 14: // y (or z) direction Lorentz force, 2x part.
				if (Frequency!=0)
				{
					B1=meshelem[i].B1;
					B2=meshelem[i].B2;
					y= (B1.re*J.re - B1.im*J.im) + I*(B1.re*J.im+B1.im*J.re);
					if(ProblemType==1) y=(-y*2.*PI*R); else y*=Depth;
					z+=(a*y)/2.;
				}
				break;

			case 16: // Lorentz Torque, 2x
				if ((Frequency!=0) && (ProblemType==0))
				{
					B1=meshelem[i].B1;
					B2=meshelem[i].B2;
					c=Ctr(i)*LengthConv[LengthUnits];
					y= c.re*((B1.re*J.re - B1.im*J.im) + I*(B1.re*J.im+B1.im*J.re))
					  +c.im*((B2.re*J.re - B2.im*J.im) + I*(B2.re*J.im+B2.im*J.re));
					z+=0.5*(a*y*Depth);
				}
				break;

			case 15: // Lorentz Torque, SS part.
				if(ProblemType==0)
				{
					B1=meshelem[i].B1;
					B2=meshelem[i].B2;
					c=Ctr(i)*LengthConv[LengthUnits];
					y= c.im*(B2.re*J.re + B2.im*J.im) + c.re*(B1.re*J.re + B1.im*J.im);
					if(Frequency!=0) y*=0.5;
					z+=(a*y*Depth);
				}
				break;

			case 1: // integrate A over the element;
				if(ProblemType==1)
					y=AxiInt(a,U,A,r);
				else
					for(k=0,y=0;k<3;k++) y+=a*Depth*A[k]/3.;

				z+=y;
				break;

			case 2: // stored energy
				if(ProblemType==1) a*=(2.*PI*R); else a*=Depth;
				B1=meshelem[i].B1;
				B2=meshelem[i].B2;
				if(Frequency!=0){
					// have to compute the energy stored in a special way for
					// wound regions subject to prox and skin effects
					if (blockproplist[meshelem[i].blk].LamType>2)
					{
						CComplex mu;
						mu=muo*blocklist[meshelem[i].lbl].mu;
						double u=Im(1./blocklist[meshelem[i].lbl].o)/(2.e6*PI*Frequency);
						y=a*Re(B1*conj(B1)+B2*conj(B2))*Re(1./mu)/4.;
						y+=a*Re(J*conj(J))*u/4.;
					}
					else y=a*blockproplist[meshelem[i].blk].DoEnergy(B1,B2);
				}
				else
				{
					// correct H and energy stored in magnet for second-quadrant
					// representation of a PM.
					if (blockproplist[meshelem[i].blk].H_c!=0)
					{
						int bk=meshelem[i].blk;

						// in the linear case:
						if (blockproplist[bk].BHpoints==0)
						{
							CComplex Hc;
							mu1=blockproplist[bk].mu_x;
							mu2=blockproplist[bk].mu_y;
							H1=B1/(mu1*muo);
							H2=B2/(mu2*muo);
							Hc = blockproplist[bk].H_c*exp(I*PI*meshelem[i].magdir/180.);
							H1=H1-Re(Hc);
							H2=H2-Im(Hc);
							y = a*0.5*muo*(mu1.re*H1.re*H1.re + mu2.re*H2.re*H2.re);
						}
						else{ // the material is nonlinear
							y=blockproplist[bk].DoEnergy(B1.re,B2.re);
							y = y + blockproplist[bk].Nrg
								  - blockproplist[bk].H_c*Re((B1.re+I*B2.re)/exp(I*PI*meshelem[i].magdir/180.));
							y*=a;
						}
					}
					else y=a*blockproplist[meshelem[i].blk].DoEnergy(B1.re,B2.re);

					// add in "local" stored energy for wound that would be subject to
					// prox and skin effect for nonzero frequency cases.
					if (blockproplist[meshelem[i].blk].LamType>2)
					{
						double u=Im(blocklist[meshelem[i].lbl].o);
						y+=a*Re(J*J)*u/2.;
					}
				}
				y*=AECF(i); // correction for axisymmetric external region;

				z+=y;
				break;

			case 3:  // Hysteresis & Laminated eddy current losses
				if(Frequency!=0){
					if(ProblemType==1) a*=(2.*PI*R); else a*=Depth;
					B1=meshelem[i].B1;
					B2=meshelem[i].B2;
					GetMu(B1,B2,mu1,mu2,i);
					H1=B1/(mu1*muo);
					H2=B2/(mu2*muo);

					y=a*PI*Frequency*Im(H1*B1.Conj() + H2*B2.Conj());
					z+=y;
				}
				break;

			case 4: // Resistive Losses
				sig=1.e06/Re(1./blocklist[meshelem[i].lbl].o);
				if((blockproplist[meshelem[i].blk].Lam_d!=0) &&
					(blockproplist[meshelem[i].blk].LamType==0)) sig=0;
				if(sig!=0){

					if (ProblemType==0){
						for(k=0;k<3;k++) V[k]=Jn[k].Conj()/sig;
						y=PlnInt(a,Jn,V)*Depth;
					}

					if(ProblemType==1)
						y=2.*PI*R*a*J*conj(J)/sig;

					if(Frequency!=0) y/=2.;
					z+=y;
				}
				break;

			case 5: // cross-section area
				z+=a;
				break;

            case 10: // volume
				if(ProblemType==1) a*=(2.*PI*R); else a*=Depth;
				z+=a;
				break;

			case 7: // total current in block;
				z+=a*J;

				break;

			case 8: // integrate x or r part of b over the block
				if(ProblemType==1) a*=(2.*PI*R); else a*=Depth;
				z+=(a*meshelem[i].B1);
				break;

			case 9: // integrate y or z part of b over the block
				if(ProblemType==1) a*=(2.*PI*R); else a*=Depth;
				z+=(a*meshelem[i].B2);
				break;

			case 17: // Coenergy
				if(ProblemType==1) a*=(2.*PI*R); else a*=Depth;
				B1=meshelem[i].B1;
				B2=meshelem[i].B2;
				if(Frequency!=0){
					// have to compute the energy stored in a special way for
					// wound regions subject to prox and skin effects
					if (blockproplist[meshelem[i].blk].LamType>2)
					{
						CComplex mu;
						mu=muo*blocklist[meshelem[i].lbl].mu;
						double u=Im(1./blocklist[meshelem[i].lbl].o)/(2.e6*PI*Frequency);
						y=a*Re(B1*conj(B1)+B2*conj(B2))*Re(1./mu)/4.;
						y+=a*Re(J*conj(J))*u/4.;
					}
					else y=a*blockproplist[meshelem[i].blk].DoCoEnergy(B1,B2);
				}
				else
				{
					y=a*blockproplist[meshelem[i].blk].DoCoEnergy(B1.re,B2.re);

					// add in "local" stored energy for wound that would be subject to
					// prox and skin effect for nonzero frequency cases.
					if (blockproplist[meshelem[i].blk].LamType>2)
					{
						double u=Im(blocklist[meshelem[i].lbl].o);
						y+=a*Re(J*J)*u/2.;
					}
				}
				y*=AECF(i); // correction for axisymmetric external region;

				z+=y;
				break;

			case 24: // Moment of Inertia-like integral

				// For axisymmetric problems, compute the moment
				// of inertia about the r=0 axis.
				if(ProblemType==1){
					for(k=0;k<3;k++) V[k]=r[k];
					y=AxiInt(a,V,V,r);
				}

				// For planar problems, compute the moment of
				// inertia about the z=axis.
				else{
					for(k=0;k<3;k++)
					{
						U[k]=meshnode[meshelem[i].p[k]].x*LengthConv[LengthUnits];
						V[k]=meshnode[meshelem[i].p[k]].y*LengthConv[LengthUnits];
					}
					y =U[0]*U[0] + U[1]*U[1] + U[2]*U[2];
					y+=U[0]*U[1] + U[0]*U[2] + U[1]*U[2];
					y+=V[0]*V[0] + V[1]*V[1] + V[2]*V[2];
					y+=V[0]*V[1] + V[0]*V[2] + V[1]*V[2];
					y*=(a*Depth/6.);
				}

				z+=y;
				break;

			default:
				break;
		}
      }

	  // integrals that need to be evaluated over all elements,
	  // regardless of which elements are actually selected.
	  if((inttype>=18) || (inttype<=23))
	  {
			a=ElmArea(i)*pow(LengthConv[LengthUnits],2.);
			if(ProblemType==1){
				for(k=0;k<3;k++)
					r[k]=meshnode[meshelem[i].p[k]].x*LengthConv[LengthUnits];
				R=(r[0]+r[1]+r[2])/3.;
				a*=(2.*PI*R);
			}
			else a*=Depth;

			switch(inttype){

				case 18: // x (or r) direction Henrotte force, SS part.
					if(ProblemType!=0) break;

					B1=meshelem[i].B1;
					B2=meshelem[i].B2;
					c=HenrotteVector(i);
					y=(((B1*conj(B1)) - (B2*conj(B2)))*Re(c) + 2.*Re(B1*conj(B2))*Im(c))/(2.*muo);
					if(Frequency!=0) y/=2.;

					y*=AECF(i); // correction for axisymmetric external region;

					z+=(a*y);
					break;

				case 19: // y (or z) direction Henrotte force, SS part.

					B1=meshelem[i].B1;
					B2=meshelem[i].B2;
					c=HenrotteVector(i);

					y=(((B2*conj(B2)) - (B1*conj(B1)))*Im(c) + 2.*Re(B1*conj(B2))*Re(c))/(2.*muo);

					y*=AECF(i); // correction for axisymmetric external region;

					if(Frequency!=0) y/=2.;
					z+=(a*y);

					break;

				case 20: // x (or r) direction Henrotte force, 2x part.

					if(ProblemType!=0) break;
					B1=meshelem[i].B1;
					B2=meshelem[i].B2;
					c=HenrotteVector(i);
					z+=a*((((B1*B1) - (B2*B2))*Re(c) + 2.*B1*B2*Im(c))/(4.*muo)) * AECF(i);

					break;

				case 21: // y (or z) direction Henrotte force, 2x part.

					B1=meshelem[i].B1;
					B2=meshelem[i].B2;
					c=HenrotteVector(i);
					z+= a*((((B2*B2) - (B1*B1))*Im(c) + 2.*B1*B2*Re(c))/(4.*muo)) * AECF(i);

					break;

				case 22: // Henrotte torque, SS part.
					if(ProblemType!=0) break;
					B1=meshelem[i].B1;
					B2=meshelem[i].B2;
					c=HenrotteVector(i);

					F1 = (((B1*conj(B1)) - (B2*conj(B2)))*Re(c) +
						 2.*Re(B1*conj(B2))*Im(c))/(2.*muo);
					F2 = (((B2*conj(B2)) - (B1*conj(B1)))*Im(c) +
						 2.*Re(B1*conj(B2))*Re(c))/(2.*muo);

					for(c=0,k=0;k<3;k++)
						c+=meshnode[meshelem[i].p[k]].CC()*LengthConv[LengthUnits]/3.;

					y=Re(c)*F2 -Im(c)*F1;
					if(Frequency!=0) y/=2.;
					y*=AECF(i);
					z+=(a*y);

					break;

				case 23: // Henrotte torque, 2x part.

					if(ProblemType!=0) break;
					B1=meshelem[i].B1;
					B2=meshelem[i].B2;
					c=HenrotteVector(i);
					F1 = (((B1*B1) - (B2*B2))*Re(c) + 2.*B1*B2*Im(c))/(4.*muo);
					F2 = (((B2*B2) - (B1*B1))*Im(c) + 2.*B1*B2*Re(c))/(4.*muo);

					for(c=0,k=0;k<3;k++)
						c+=meshnode[meshelem[i].p[k]].CC()*LengthConv[LengthUnits]/3;

					z+=a*(Re(c)*F2 -Im(c)*F1)*AECF(i);

					break;

				default:
					break;
		  }
	  }
	}

	return z;
}