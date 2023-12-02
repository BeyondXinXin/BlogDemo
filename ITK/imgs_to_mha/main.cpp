#include <QHash>
#include <QDebug>

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkGDCMImageIO.h>
#include <itkImageSeriesReader.h>
#include <itkNumericSeriesFileNames.h>
#include <itkJPEGImageIO.h>

static const QString filePath = "E:/dcm_data/tmp_imgs/";
int main(int argc, char* argv[])
{
	using PixelType = unsigned short;
	constexpr unsigned int itk_dimension = 3;
	using ImageType = itk::Image<PixelType, itk_dimension>;
	using ReaderType = itk::ImageSeriesReader<ImageType>;
	using WriterType = itk::ImageFileWriter<ImageType>;
	using NameGeneratorType = itk::NumericSeriesFileNames;
	ReaderType::Pointer itk_reader_png = ReaderType::New();
	WriterType::Pointer itk_writer_mhd = WriterType::New();
	NameGeneratorType::Pointer itk_nameGenerator = NameGeneratorType::New();
	itk_nameGenerator->SetSeriesFormat((filePath + QString("tmp%05d.jpg")).toLocal8Bit().data());
	itk_nameGenerator->SetStartIndex(static_cast<unsigned long>(1));
	itk_nameGenerator->SetEndIndex(static_cast<unsigned long>(98));
	itk_nameGenerator->SetIncrementIndex(static_cast<unsigned long>(1));
	itk_reader_png->SetImageIO(itk::JPEGImageIO::New());
	itk_reader_png->SetFileNames(itk_nameGenerator->GetFileNames());
	itk_writer_mhd->SetFileName("./1.mha");
	itk_writer_mhd->SetInput(itk_reader_png->GetOutput());
	try {
		itk_writer_mhd->Update();
	}
	catch (itk::ExceptionObject& err) {
		qDebug() << "fail:" << err.GetDescription();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
