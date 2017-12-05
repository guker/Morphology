#include <iostream>
#include <fstream> // class necessary to read files
using namespace std;

class morphology {
		
		private:
		
			int numRowsIMG, numColsIMG, minValIMG, maxValIMG;
	  		int numRowsStrctElem, numColsStrctElem, minValStrctElem, maxValStrctElem;
	  		int rowOrigin, colOrigin;
			int rowFrameSize, colFrameSize;
			int** imgAry;	
			int** backupImgAry;
			int** morphAry;	
	     	int** structElem;
			char* inFile1;
			char* inFile2;
			char* outFile1;
			char* outFile2;
			char* outFile3;
			char* outFile4;
	
	    public:
	    	
			morphology(	char* inFile1, char* inFile2, char* outFile1,
						char* outFile2, char* outFile3, char* outFile4) {
								
				this->inFile1 = inFile1;
				this->inFile2 = inFile2;
				this->outFile1 = outFile1;
				this->outFile2 = outFile2;
				this->outFile3 = outFile3;
				this->outFile4 = outFile4;
				
				loadStruct();
				computeFrameSize();
				loadImage();
		
				buildMorphAry();
				zeroOutMorphAry();
				
			} 
	
			void loadStruct() {
			
				ifstream myfile(inFile2);
				
				myfile >> numRowsStrctElem;
				myfile >> numColsStrctElem;
				myfile >> minValStrctElem;
				myfile >> maxValStrctElem;
				
				myfile >> rowOrigin;
				myfile >> colOrigin;
				
				structElem = new int*[numRowsStrctElem];
				for(int i = 0; i < numRowsStrctElem; i++)
					structElem[i] = new int[numColsStrctElem];
		 
		  		for(int i = 0; i < numRowsStrctElem; i++) {
		  			for(int j = 0; j < numColsStrctElem; j++)
		      			myfile >> structElem[i][j];
				}
				
				myfile.close(); // close input file
			
			}
			
			void computeFrameSize() {
				rowFrameSize = numRowsStrctElem;
				colFrameSize = numColsStrctElem;
			}
			
			void loadImage() {
				
				ifstream myfile(inFile1);
				
				myfile >> numRowsIMG;
				myfile >> numColsIMG;
				myfile >> minValIMG;
				myfile >> maxValIMG;
				
				imgAry = new int*[numRowsIMG + rowFrameSize];
				
				for(int i = 0; i < numRowsIMG + rowFrameSize; i++)
					imgAry[i] = new int[numColsIMG + colFrameSize];
		 
		 		backupImgAry = new int*[numRowsIMG + rowFrameSize];
				
				for(int i = 0; i < numRowsIMG + rowFrameSize; i++)
					backupImgAry[i] = new int[numColsIMG + colFrameSize];
		 
		 		zeroFrameImage();
		 
		  		for(int i = rowFrameSize/2; i < (numRowsIMG+(rowFrameSize/2)); i++) {
		  			for(int j = colFrameSize/2; j < (numColsIMG+(colFrameSize/2)); j++) {
						myfile >> imgAry[i][j];
						backupImgAry[i][j] = imgAry[i][j];
					}
				}
							
				myfile.close();
			}
			
			void zeroFrameImage() {
				for(int i = 0; i < numRowsIMG+rowFrameSize; i++) {
		  			for(int j = 0; j < numColsIMG+colFrameSize; j++) {
					  	imgAry[i][j] = 0;
					  	backupImgAry[i][j] = 0;
					}
				}
			}
			
			void buildMorphAry() {
				morphAry = new int*[numRowsIMG + rowFrameSize];
				for(int i = 0; i < numRowsIMG + rowFrameSize; i++)
					morphAry[i] = new int[numColsIMG + colFrameSize];
			}
			
			void zeroOutMorphAry() {
				for(int i = 0; i < numRowsIMG+rowFrameSize; i++) {
		  			for(int j = 0; j < numColsIMG+colFrameSize; j++)
		      			morphAry[i][j] = 0;
				}
			}
			
			void processDilation() {
				for(int i = rowFrameSize/2; i < (numRowsIMG+(rowFrameSize/2)); i++) {
					for(int j = colFrameSize/2; j < (numColsIMG+(colFrameSize/2)); j++) {
						if (imgAry[i][j] > 0) {
							dilation(i, j);
						}
					}
				}
			}

			void dilation(int row, int col) {
				
				for(int imgRow = row-rowOrigin, strctRow = 0; 
						strctRow < numRowsStrctElem; 
						imgRow++, strctRow++) {
		  			
					for(int imgCol = col-colOrigin,	strctCol = 0;
							strctCol < numColsStrctElem;
							imgCol++, strctCol++) {
								
							if (structElem[strctRow][strctCol] > 0) {
		      					morphAry[imgRow][imgCol] = 1;
							}
					}
				}
			}
			
			
			void processErosion() {
				for(int i = rowFrameSize/2; i < (numRowsIMG+(rowFrameSize/2)); i++) {
					for(int j = colFrameSize/2; j < (numColsIMG+(colFrameSize/2)); j++) {
						if (imgAry[i][j] > 0) {
							erosion(i, j);
						}
					}
				}
			}
			
			void erosion(int row, int col) {
				
				bool flag = true;

				for(int imgRow = row-rowOrigin, strctRow = 0; 
						strctRow < numRowsStrctElem; 
						imgRow++, strctRow++) {
		  			
					for(int imgCol = col-colOrigin,	strctCol = 0;
							strctCol < numColsStrctElem;
							imgCol++, strctCol++) {
								
							if (structElem[strctRow][strctCol] == 1 && imgAry[imgRow][imgCol] != 1) {
		      					flag = false;
		      					break;
							}
					}	
				}
				if(flag) morphAry[row][col] = 1;
			}
			
			void updateInputImage() {
				for(int i = 0; i < numRowsIMG+rowFrameSize; i++) {
		  			for(int j = 0; j < numColsIMG+colFrameSize; j++) {
						imgAry[i][j] = morphAry[i][j];
					}		
				}
			}
			
			void restoreInputImage() {
				for(int i = 0; i < numRowsIMG+rowFrameSize; i++) {
		  			for(int j = 0; j < numColsIMG+colFrameSize; j++) {
						imgAry[i][j] = backupImgAry[i][j];
					}		
				}
			}
			
			void processClosing() {
				processDilation();
				updateInputImage();
				zeroOutMorphAry();
				processErosion();
				restoreInputImage();
			}
		
			void processOpening() {
				processErosion();
				updateInputImage();
				zeroOutMorphAry();
				processDilation();
				restoreInputImage();
			}
			
			void prettyPrintImgAry() {
				
				cout << "Input Image" << endl;
				cout << "===========" << endl;
				
				for(int i = 0; i < numRowsIMG+rowFrameSize; i++) {
		  			for(int j = 0; j < numColsIMG+colFrameSize; j++) {
						if (imgAry[i][j] > 0) cout << imgAry[i][j] << " ";
						else cout << "  ";
					}
					cout << endl;
				}
				cout << endl;
			}
			
			void prettyPrintStructElem() {

				cout << "Structuring Element" << endl;
				cout << "===================" << endl;
								
				for(int i = 0; i < numRowsStrctElem; i++) {
		  			for(int j = 0; j < numColsStrctElem; j++) {
		  				if (structElem[i][j] > 0) cout << structElem[i][j] << " ";
						else cout << "  ";
					}
		      		cout << endl;
				}
				cout << endl;
			}
			
			void prettyPrintMorphAry(string result) {
				
				cout << result << endl;
				cout << "===============" << endl;
				
				for(int i = 0; i < numRowsIMG+rowFrameSize; i++) {
		  			for(int j = 0; j < numColsIMG+colFrameSize; j++) {
						if (morphAry[i][j] > 0) cout << morphAry[i][j] << " ";
						else cout << "  ";
					}
					cout << endl;
				}
				cout << endl;
				
			}
			
			void outPutResult(int fileNum) {
				
				ofstream myoutfile;
				
				if(fileNum == 1) myoutfile.open(outFile1);
				if(fileNum == 2) myoutfile.open(outFile2);
				if(fileNum == 3) myoutfile.open(outFile3);
				if(fileNum == 4) myoutfile.open(outFile4);
				
				myoutfile << numRowsIMG << " " << numColsIMG << " " << minValIMG << " " << maxValIMG << endl;
				
				for(int i = rowFrameSize/2; i < (numRowsIMG+(rowFrameSize/2)); i++) {
					for(int j = colFrameSize/2; j < (numColsIMG+(colFrameSize/2)); j++) {
						myoutfile << morphAry[i][j] << " ";
					}
					myoutfile << endl;
				}			
				myoutfile.close();
			}
			
};


int main(int argc, char *argv[]) {

	morphology m(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);

	m.prettyPrintImgAry();
	m.prettyPrintStructElem();
	
	// Dilation
	
	m.processDilation();
	m.prettyPrintMorphAry("Dilation Result");
	m.outPutResult(1);
	m.zeroOutMorphAry();
	
	// Erosion
	
	m.processErosion();
	m.prettyPrintMorphAry("Erosion Result");
	m.outPutResult(2);
	m.zeroOutMorphAry();
	
	// Closing
	
	m.processClosing();
	m.prettyPrintMorphAry("Closing Result");
	m.outPutResult(3);
	m.zeroOutMorphAry();
	
	// Opening
	
	m.processOpening();
	m.prettyPrintMorphAry("Opening Result");
	m.outPutResult(4);

	return 0;

} // end of main


