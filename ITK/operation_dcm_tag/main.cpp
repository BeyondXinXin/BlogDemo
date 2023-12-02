#include <QHash>
#include <QDebug>

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkGDCMImageIO.h>

static const QString fileName = "E:/dcm_data/CTCAP/1.000000-Topogram  1.0  T20s-63493/1-1.dcm";

void ReadDcmTag() {
	// DCM –≈œ¢∂¡»°
	QHash<QString, QString> dcm_tig;

	using PixelType = signed short;
	constexpr unsigned int dimension = 2;
	using ImageType = itk::Image<PixelType, dimension>;
	using ReaderType = itk::ImageFileReader<ImageType>;
	using ImageIOType = itk::GDCMImageIO;

	ReaderType::Pointer reader = ReaderType::New();
	ImageIOType::Pointer dicom_io = ImageIOType::New();
	reader->SetFileName(fileName.toLocal8Bit().data());
	reader->SetImageIO(dicom_io);
	try {
		reader->Update();
	}
	catch (itk::ExceptionObject& ex) {
		qDebug() << "dicom open fail " << ex.GetDescription();
		return;
	}

	using DictionaryType = itk::MetaDataDictionary;
	const DictionaryType& dictionary = dicom_io->GetMetaDataDictionary();
	using MetaDataStringType = itk::MetaDataObject<std::string>;

	for (auto ite = dictionary.Begin(); ite != dictionary.End(); ++ite) {
		QString id = QString::fromStdString(ite->first);
		itk::MetaDataObjectBase::Pointer entry = ite->second;
		MetaDataStringType::ConstPointer entry_value =
			dynamic_cast<const MetaDataStringType*>(ite->second.GetPointer());
		std::string key_string;
		itk::GDCMImageIO::GetLabelFromTag(id.toStdString().c_str(), key_string);
		QString key = QString::fromStdString(key_string);
		if (key.isEmpty()) {
			break;
		}
		QString value = QString::fromStdString(entry_value->GetMetaDataObjectValue());
		dcm_tig.insert(key, value);
	}

	QHash<QString, QString> ::const_iterator it;
	for (it = dcm_tig.begin(); it != dcm_tig.end(); it++) {
		qDebug() << it.key() << "," << it.value();
	}
}

void WriteDcmTag() {
	using PixelType = unsigned short;
	constexpr unsigned int Dimension = 2;

	using ImageType = itk::Image<PixelType, Dimension>;
	using ReaderType = itk::ImageFileReader<ImageType>;
	using WriterType = itk::ImageFileWriter<ImageType>;

	ReaderType::Pointer reader = ReaderType::New();
	WriterType::Pointer writer = WriterType::New();

	reader->SetFileName(fileName.toLocal8Bit().data());

	try
	{
		reader->Update();
	}
	catch (itk::ExceptionObject& ex)
	{
		qDebug() << "dicom open fail " << ex.GetDescription();
		return;
	}

	ImageType::Pointer image = reader->GetOutput();
	itk::MetaDataDictionary& dictionary = image->GetMetaDataDictionary();
	std::string newPatientName = "aaa";
	itk::EncapsulateMetaData<std::string>(dictionary, "0010|0010", newPatientName);
	writer->SetFileName("./1.dcm");
	writer->SetInput(image);
	try
	{
		writer->Update();
	}
	catch (itk::ExceptionObject& ex)
	{
		qDebug() << "dicom write fail " << ex.GetDescription();
		return;
	}
}


int main(int argc, char* argv[])
{
	ReadDcmTag();

	WriteDcmTag();

	return EXIT_SUCCESS;
}
