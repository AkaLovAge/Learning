#include <vector>
#include <fstream>
#include <iostream>
#include <typeinfo>
#include <math.h>
#include "gdal_priv.h"
#include "cpl_conv.h"
#include "liblas/liblas.hpp"
#include "ogr_spatialref.h"
#include <cstring>
#include <sstream>

// if the point is the last return
/*class LastReturnFilter: public liblas::FilterI
{
public:
	LastReturnFilter();
	bool filter (const liblas::Point& p);
private:
	LastReturnFilter(LastReturnFilter const& other);
	LastReturnFilter& operator=(LastReturnFilter const& rhs);
};

LastReturnFilter::LastReturnFilter( ) : liblas::FilterI(eInclusion){}
 
bool LastReturnFilter::filter(const liblas::Point& p)
{
	bool output = false;
	if (p.GetReturnNumber() == p.GetNumberOfReturns()){
		output = true;
	}
	if (GetType() == eExclusion && output == true)
	{
		output = false;
	}else{
		output = true;
	}
	return output;
}
*/
void Raster_writer (char const* out_file, double *data, int row,int col,double topleft_x, double topleft_y)
{
	double affine[] = {topleft_x, 1, 0, topleft_y, 0, -1};
    const char *pszFormat = "GTiff";
	GDALAllRegister();
    GDALDriver *poDriver;
    char **papszMetadata;
    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
    //if( poDriver == NULL )
      //  exit( 1 );
    papszMetadata = poDriver->GetMetadata();
	std::cout<< "the input parameter is:"<< out_file << '\n';
    if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
        printf( "Driver %s supports Create() method.\n", pszFormat );
    if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) )
        printf( "Driver %s supports CreateCopy() method.\n", pszFormat );
	GDALDataset *poDstDS;
	char **papszOptions = NULL;
	std::cout<< row << "," << col <<'\n';
/*	for (int i = 0; i< row; i++)
    {
        for (int j = 0; j < col; j++)
        { 
           std::cout << data[i * col + j] << ",   ";
        }
        std::cout << std::endl;
    }
*/
	poDstDS = poDriver->Create( out_file,col, row, 1, GDT_Float64,papszOptions );
	OGRSpatialReference oSRS;
	char *pszSRS_WKT = NULL;
	GDALRasterBand *poBand;
	
	poDstDS->SetGeoTransform( affine );
	oSRS.SetUTM( 15, TRUE );
	oSRS.SetWellKnownGeogCS( "NAD83" );
	oSRS.exportToWkt( &pszSRS_WKT );
	poDstDS->SetProjection( pszSRS_WKT );
	CPLFree( pszSRS_WKT );
	poBand = poDstDS->GetRasterBand(1);
	poBand->RasterIO( GF_Write, 0, 0, col,row,data,col,row, GDT_Float64, 0, 0 );
/*Once we're done, close properly the dataset */
	GDALClose( (GDALDatasetH) poDstDS );	
/*	GDALDatasetH hDstDS;
	GDALRasterBandH hBand;
	OGRSpatialReferenceH hSRS;
	GDALDriverH hDriver;
	
	char *pszSRS_WKT = NULL;
	char **papszOptions = NULL;
	
	hDriver = GDALGetDriverByName("GTiff");
	hDstDS = GDALCreate(hDriver, out_file, col, row, 1, GDT_Float32, papszOptions);
	
	GDALSetGeoTransform(hDstDS, affine);
	
	hSRS = OSRNewSpatialReference(NULL);
	OSRImportFromProj4(hSRS, "+proj=utm +zone=15 +ellps=GRS80 +datum=NAD83 +units=m +no_defs");
	OSRExportToWkt(hSRS, &pszSRS_WKT);
	OSRDestroySpatialReference(hSRS);
	GDALSetProjection(hDstDS, pszSRS_WKT);
	CPLFree(pszSRS_WKT);
	hBand = GDALGetRasterBand(hDstDS,1);
	GDALSetRasterNoDataValue(hBand, );
	GDALRasterIO(hBand, GF_Write, 0, 0, col, row, data, col, row, GDT_Float32, 0, 0);
	GDALClose(hDstDS);
*/			
}

double mean_calculator(double *zvalue)
{	
	double sum = 0;
	bool isNull = true; 
	int count = 0;
	for (int i=0; i<10; i++)
	{
		if (zvalue[i] < DBL_MAX){
			sum += zvalue[i];
			count += 1;
			isNull = false;
		}
	}
	if (isNull){
		return 0;
	}else{
		return (sum/count);
	}
}
struct grid 
{
	int row_num;
	int col_num;
	double *data;
};
struct raster
{
	grid Grid;
	char *reference;
	double affine [6];
};

int main(int argc, char* argv[] ){
for (int file_count =1; file_count<10; file_count++)
{
	int DEM_Resolution = 1;
	double *DEM_Raster; // the final dem raster 
    double **Raster_Zvalue; // the array of z value in each grid 
	double minX,minY,maxX,maxY;
	std::ifstream ifs;
	std::stringstream ss;
	ss.str("");
    ss.clear();
	ss << file_count;
	char path[1025] = {};
	strcat (path,"Jean-Lafitte/LA_Jean-Lafitte_2013_00000");
	strcat (path,const_cast<char*>(ss.str().c_str()));
	strcat (path,".las");
	std::cout << path << '\n';
	ifs.open(path,std::ios::in | std::ios::binary);
//	{
//		std::cout << "Can not open file" << std::endl;
//		exit(1);
//	}
	
	std::vector<liblas::FilterPtr> filters;
//	liblas::FilterPtr lastreturn_filter = liblas::FilterPtr(new LastReturnFilter());
//	filters.push_back(lastreturn_filter);
	
	liblas::ReaderFactory f;
	liblas::Reader reader = f.CreateWithStream(ifs);
//	reader.SetFilters(filters);
	liblas::Header const& header = reader.GetHeader();

	std::cout << "Compressed: " << (header.Compressed() == true) ? "true":"false";
	std::cout << "Signature: " << header.GetFileSignature() << '\n';
	std::cout << "Points count:" << header.GetPointRecordsCount() << '\n';
	std::cout << "Project ID" << header.GetProjectId() << '\n';
	
	liblas::Bounds<double> CloudBounds = header.GetExtent();
	std::cout << "X scale:"<< header.GetMaxX() << "," << header.GetMinX() << '\n';
	std::cout << "X offset" << header.GetOffsetX() << '\n';
	std::cout << "X Scale" << header.GetScaleX() << '\n';
		
	minX = header.GetMinX();
	minY = header.GetMinY();
	maxX = header.GetMaxX();
	maxY = header.GetMaxY();

	int row_num = ceil((maxY - minY) / DEM_Resolution);// * 0.03048);
	int col_num = ceil((maxX - minX) / DEM_Resolution);// * 0.03048);
	
	std::cout <<"Output Raster is:" << row_num << "*" << col_num << '\n';		
 
	DEM_Raster = new double [row_num * col_num];
	Raster_Zvalue = new double* [row_num * col_num];
	
	for (int i=0; i<row_num * col_num ; i++)
	{
		Raster_Zvalue [i] = new double [10];
	}

	for (int i=0; i<row_num * col_num; i++)
	{
		for (int j=0; j<10;j++)
		{
			Raster_Zvalue[i][j] = DBL_MAX;
		}
	}
//	std::vector<liblas::Point> PointCloud;

	while (reader.ReadNextPoint())
	{
		liblas::Point const& p = reader.GetPoint();
		if (p.GetReturnNumber() == p.GetNumberOfReturns())
		{	
			double coord_x = p.GetX();
			double coord_y = p.GetY();
			double z_value = p.GetZ();
//			std::cout << coord_x << "," << coord_y << "," << p.GetReturnNumber() << "," << p.GetNumberOfReturns() << std::endl;	
			
			int DEM_coord_y = floor((coord_x - minX) / DEM_Resolution);//* 0.03048);
			int DEM_coord_x = floor((coord_y - minY) / DEM_Resolution);//* 0.03048);

		//	std::cout << DEM_coord_x << "," << DEM_coord_y << '\n';
			if (z_value < Raster_Zvalue[DEM_coord_x * col_num + DEM_coord_y][9])
			{
				for (int m=0 ; m<10; m++)
				{
					if (z_value < Raster_Zvalue[DEM_coord_x * col_num + DEM_coord_y][m])
					{	
						for (int n=9; n>m; n-- )
						{
							Raster_Zvalue[DEM_coord_x * col_num + DEM_coord_y][n] = Raster_Zvalue[DEM_coord_x * col_num + DEM_coord_y][n-1];
						}
						Raster_Zvalue[DEM_coord_x * col_num + DEM_coord_y][m] = z_value;
						break;
					}	
				}
			}
		}
	}

	for (int i=0; i<row_num; i++)
	{
		for (int j=0; j<col_num; j++)
		{
			DEM_Raster[i * col_num + j] = mean_calculator(Raster_Zvalue[i*col_num+j]);
		}
	}
	char out_put[1024] = {};
	strcat (out_put,"Output_");
	std::cout << out_put<<'\n';
	strcat (out_put,const_cast<char*>(ss.str().c_str()));
	strcat (out_put,".tif");
	Raster_writer(out_put , DEM_Raster, row_num, col_num, minX, maxY);
// release Raster_Zvalue
	std:: cout << out_put << path << '\n';	
	
	for (int i=0 ; i<row_num*col_num ; i++)
	{
		delete [] Raster_Zvalue[i]; 
	}
	delete [] Raster_Zvalue;

// release DEM_Raster
	delete [] DEM_Raster;
}
	return 0;
}
