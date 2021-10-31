/********************************************************************/
/*                                                                  */
/*        Funktion zur Prüfung, ob die übergebenen Strecken         */
/*                A und B sich gegenseitig schneiden                */
/*                                                                  */
/*   Rückgabewerte:                                                 */
/*   0 -> kein Schnittpunkt                                         */
/*   1 -> Strecken hängen aneinander, Berührung am Anfang/Ende      */
/*   2 -> ein Streckenendpunkt berührt innerhalb der anderen Strecke*/
/*   3 -> 'echter' Schnittpunkt                                     */
/*   4 -> unendlich viele Schnittpunkte                             */
/*                                                                  */
/********************************************************************/

int _stdcall CGraphicsMath::CheckLineSectionCut( POINT& A1, POINT& A2, POINT& B1, POINT& B2 )
{
   int      nDetLambda, nDetNenner;
   int      ret;
   POINT    A21, B12, B1A1;

   A21.x = A2.x - A1.x;                          // Richtungsvektoren für Strecken A und B
   A21.y = A2.y - A1.y;                          // A1 und B1 sind die Anfangspunkte der Strecken
   B12.x = B1.x - B2.x;
   B12.y = B1.y - B2.y;

   B1A1.x = B1.x - A1.x;                         // Differenzvektor der Streckenanfangspunkte
   B1A1.y = B1.y - A1.y;
                                                 // Punktekoordinaten sind < 10000 !! (kein Überlauf)
   nDetNenner = A21.x*B12.y - A21.y*B12.x;       // Nenner-Determinante der Cramer-Regel
   nDetLambda = B1A1.x*B12.y - B1A1.y*B12.x;     // Zähler-Determinante für Lambda der Strecke A
   
   if( nDetNenner != 0 )                         // Strecken A und B sind NICHT parallel
   {
      bool   blambda   =true ,bmy   =true;       // true entspricht lamdba,my genau 0 oder 1
      bool   blambda021=false,bmy021=false;      // true entspricht lambda,my zwischen 0 und 1

      int nDetMy = A21.x*B1A1.y - A21.y*B1A1.x;  // Zähler-Determinante für My der Strecke B

                                                 // Lambda, My sind die Geradengleichungs-Parameter
                                                 // dlambda wird nicht genau 0 oder 1
      if((nDetLambda!=0)&&(nDetLambda!=nDetNenner))
      {
         double dLambda = ((double) nDetLambda)/((double) nDetNenner);
         blambda        = false;
         blambda021     = (dLambda>0.0)&&(dLambda<1.0);
      }

      if((nDetMy!=0)&&(nDetMy!=nDetNenner))
      {
         double dMy = ((double) nDetMy)/((double) nDetNenner);
         bmy        = false;
         bmy021     = (dMy>0.0)&&(dMy<1.0);
      }

      if(blambda&bmy)
         ret = 1;                                // die Strecken A und B hängen aneinander
      else if(blambda&bmy021)
         ret = 2;                                // ein Streckenende(lambda) berührt innerhalb der anderen Strecke
      else if(bmy&blambda021)
         ret = 2;                                // ein Streckenende(my) berührt innerhalb der anderen Strecke
      else if(bmy021&blambda021)
         ret = 3;                                // Schnittpunkt zwischen beiden Strecken
      else
         ret = 0;                                // kein Schnittpunkt
   }
   else                                          // Strecken A und B sind parallel
   {
      if( nDetLambda == 0 )                      // die Strecken A und B liegen auf der gleichen Geraden
      {
         int  axy1,axy2,bxy1,bxy2;
         
         if(abs(A21.x) > abs(A21.y))             // die Strecke A zeigt eher in x-Richtung
         {
            if(A1.x < A2.x) axy1=A1.x,axy2=A2.x;
            else            axy1=A2.x,axy2=A1.x;

            if(B1.x < B2.x) bxy1=B1.x,bxy2=B2.x;
            else            bxy1=B2.x,bxy2=B1.x;
         }
         else                                    // die Strecke A zeigt eher in y-Richtung oder hat 45°
         {
            if(A1.y < A2.y) axy1=A1.y,axy2=A2.y;
            else            axy1=A2.y,axy2=A1.y;

            if(B1.y < B2.y) bxy1=B1.y,bxy2=B2.y;
            else            bxy1=B2.y,bxy2=B1.y;
         }

         if((axy2 <bxy1)||(axy1 >bxy2)) ret = 0; // beide Strecken berühren sich nicht
         else
         if((axy2==bxy1)||(axy1==bxy2)) ret = 1; // beide Strecken hängen aneinander
         else                           ret = 4; // die Strecken A und B überlappen sich -> unendlich viele Schnittpunkt
      }
      else ret = 0;                              // die parallelen Strecken schneiden sich im Unendlichen
   }

   return (ret);
}

