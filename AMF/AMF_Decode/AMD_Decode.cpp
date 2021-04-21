#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#if defined(WIN32) || defined(_WIN32) 
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")
#endif

#include "common\AMFFactory.h"
#include "include\components\VideoDecoderUVD.h"

class CDecode {
public:
	bool Init(int iW, int iH);
	bool Decode(unsigned char* data, int iLen, FILE* f);
	void WritePlane(amf::AMFPlane* plane, FILE* f);

	amf::AMFContextPtr      context;
	amf::AMFComponentPtr    decoder;
	amf::AMFBufferPtr		pictureBuffer;
	int		m_iFrames;

};
void CDecode::WritePlane(amf::AMFPlane* plane, FILE* f)
{
	// write NV12 surface removing offsets and alignments
	amf_uint8* data = reinterpret_cast<amf_uint8*>(plane->GetNative());
	amf_int32 offsetX = plane->GetOffsetX();
	amf_int32 offsetY = plane->GetOffsetY();
	amf_int32 pixelSize = plane->GetPixelSizeInBytes();
	amf_int32 height = plane->GetHeight();
	amf_int32 width = plane->GetWidth();
	amf_int32 pitchH = plane->GetHPitch();

	for (amf_int32 y = 0; y < height; y++)
	{
		amf_uint8* line = data + (y + offsetY) * pitchH;
		fwrite(reinterpret_cast<char*>(line) + offsetX * pixelSize, 1, pixelSize * width, f);
	}
}

bool CDecode::Decode(unsigned char* data, int iLen, FILE* f)
{
	amf::AMFDataPtr dTemp = NULL;
	AMF_RESULT re = AMF_OK;

	if (!pictureBuffer)
	{
		re = context->AllocBuffer(amf::AMF_MEMORY_HOST, iLen, &pictureBuffer);
		if (re != AMF_OK)
		{
			printf("AllocBuffer Failed\n");
		}
	}
	memcpy(pictureBuffer->GetNative(), data, iLen);

	dTemp = pictureBuffer.Detach();

	// Input
	re = decoder->SubmitInput(dTemp);
	if (re == AMF_OK)
	{
		// Output
		re = decoder->QueryOutput(&dTemp);
		if (re == AMF_OK)
		{
			if (dTemp)
			{
				// convert CPU memory
				re = dTemp->Convert(amf::AMF_MEMORY_HOST); // convert to system memory

				amf::AMFSurfacePtr surface(dTemp); // query for surface interface

				WritePlane(surface->GetPlane(amf::AMF_PLANE_Y), f);
				WritePlane(surface->GetPlane(amf::AMF_PLANE_UV), f);
				m_iFrames++;
			}
			else std::cout << "pictureBuffer NULL" << std::endl;
		}
		else std::cout << "QueryOutput Failed" << std::endl;
	}
	else std::cout << "SubmitInput Failed" << std::endl;
	return true;
}
bool CDecode::Init(int iW, int iH)
{
	AMF_RESULT re = AMF_OK;

	re = g_AMFFactory.Init();
	if (re != AMF_OK)
	{
		printf("Init Failed\n");
	}

	re = g_AMFFactory.GetFactory()->CreateContext(&context);
	if (re != AMF_OK)
	{
		printf("CreateContext Failed\n");
	}

	re = g_AMFFactory.GetFactory()->CreateComponent(context, AMFVideoDecoderUVD_H264_AVC, &decoder);
	if (re != AMF_OK)
	{
		printf("CreateComponent Failed\n");
	}

	re = decoder->SetProperty(AMF_VIDEO_DECODER_REORDER_MODE, AMF_VIDEO_DECODER_MODE_LOW_LATENCY);//如果不设置的话前几帧数据解析不会返回
	if (re != AMF_OK)
	{
		printf("SetProperty Failed\n");
	}

	re = decoder->Init(amf::AMF_SURFACE_NV12, iW, iH);
	if (re != AMF_OK)
	{
		printf("decode Init Failed\n");
	}
	m_iFrames = 0;
	pictureBuffer = nullptr;

	return true;
}

char buf[1920 * 1080 * 4];
union bufInfo {
	int iSize;
	char cSize[4];
};

int main(int argc, char* argv[]) {

	FILE* pFile = NULL;

	//if (argv[1] == NULL) {
	//	std::cout << argv[0] << " <ip>" << std::endl;
	//	system("PAUSE");
	//	return 0;
	//}

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		WSACleanup();
	}


	int result = fopen_s(&pFile, "output.nv12", "wb+");
	if (result != 0) {
		std::cout << __LINE__ << std::endl;
		system("PAUSE");
		return 0;
	}

	CDecode dlg;
	dlg.Init(1920, 1080);

	SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(15321);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		std::cout << "socket connect failed" << std::endl;
		return 0;
	}

	bufInfo info = { 0, };
	int totalRecv = 0;
	while (recv(sockfd, info.cSize, sizeof(bufInfo), 0) > 0) {
		while (totalRecv < info.iSize) {
			int recvSize = 0;
			recvSize = recv(sockfd, buf + totalRecv, info.iSize - totalRecv, 0);
			if (totalRecv <= 0) {
				break;
			}
			totalRecv += recvSize;
			break;
		}
		dlg.Decode((unsigned char*)buf, info.iSize, pFile);
		totalRecv = 0;

		memset(info.cSize, 0, sizeof(bufInfo));
	}
	closesocket(sockfd);
	fclose(pFile);
	WSACleanup();

	return 0;
}