#include <iostream>
#include "stdio.h"
#include "mpi.h"
#include "gdal_priv.h"
#include "cpl_conv.h"
#include "math.h"
#include <sstream>

int main (int argc,char *argv[])
{	
	GDALDataset *poDataset;
	GDALAllRegister();
	
	poDataset = (GDALDataset *) GDALOpen( "./cumberlan.tif", GA_ReadOnly );
	
	if ( poDataset == NULL )
	{
		std::cout<<"Error When TIFF File Open!"<<std::endl;
		return -1;
	};
	GDALRasterBand *poBand;
	poBand = poDataset->GetRasterBand(1);
	
	int x_size = poDataset->GetRasterXSize();
	int y_size = poDataset->GetRasterYSize();
	int band_size = poDataset->GetRasterCount();
	
	MPI_Init(&argc,&argv);

/*	if (argc < 2){
		std::cout<<"usage:"<<argv[0]<<"raster.tiff"<<std::endl;
		return -1;
	}
*/
	int rank,process_count;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	int tmp_processNum = process_count;
	int cut_num=0;
	//printf("the processor number is %d",process_count);
	int k=-1;
	while (tmp_processNum >0)
	{
		k+=2;
		tmp_processNum -= k;
	}
	if (tmp_processNum==0){
		cut_num = ceil(sqrt(process_count)); 
    }else{
		printf("ERROR! the processor is not binary square");
		return -1;	
	};
	MPI_Barrier(MPI_COMM_WORLD);
	std::cout<<"Read Raster"<<std::endl;	
	
	int cell_xNum =	(int)ceil(x_size/cut_num); 
	int cell_yNum = (int)ceil(y_size/cut_num);
	

	if ((rank%cut_num)*cell_xNum > x_size)
		cell_xNum = x_size - (cut_num-1)*cell_xNum;

    if (floor(rank/cut_num)*cell_yNum > y_size)
		cell_yNum = y_size - (cut_num-1)*cell_yNum;
	printf ("Read From %d,%d,%d,%d\n",cell_xNum*(rank%cut_num),(int)cell_yNum*(floor(rank/cut_num)),cell_xNum,cell_yNum);

	float *pafScanline = (float *) CPLMalloc(sizeof(float)*cell_xNum*cell_yNum);
	int x_block = cell_xNum * (rank % cut_num);
	int y_block = cell_yNum * floor(rank / cut_num);	
	printf("[%i] Rank has block dims: [%i, %i] of total [%i,%i]\n", rank, x_block, y_block, cell_xNum, cell_yNum);
	poBand->RasterIO(GF_Read, 0, 0 ,cell_xNum,cell_yNum,pafScanline,cell_xNum,cell_yNum,GDT_Float32,0,0);

	printf ("%d of %d\n", rank, process_count);
	printf ("cut:%d,x:%d,y:%d\n",cut_num,x_size,y_size);
	printf("%d pixel is %f\n",1,pafScanline[1]);
	CPLFree(pafScanline);
	MPI_Finalize();
	GDALClose(poDataset);
	return 0;
}
