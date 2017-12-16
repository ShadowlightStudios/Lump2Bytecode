#include <iostream>
#include <string>
#include <cstdio>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

// Define types of lumps
// "LT_" stands for "lump type"
#define LT_EMPTY						0x0000	
#define LT_VERTEX_SHADER				0x0001
#define LT_FRAGMENT_SHADER				0x0002
#define LT_GEOMETRY_SHADER				0x0003
#define LT_PROGRAM						0x0004
#define LT_UNIFORM						0x0005
#define LT_BUFFER						0x0006
#define LT_VERTEX_ARRAY					0x0007
#define LT_TEXTURE_1D					0x0008
#define LT_TEXTURE_2D					0x0009
#define LT_TEXTURE_3D					0x000A
#define LT_TEXTURE_RECT					0x000B
#define LT_TEXTURE_CUBE					0x000C
#define LT_TEXTURE_1D_ARRAY				0x000D
#define LT_TEXTURE_2D_ARRAY				0x000E
#define LT_TEXTURE_CUBE_ARRAY			0x000F
#define LT_TEXTURE_2D_MULTISAMPLE		0x0010
#define LT_TEXTURE_2D_MULTISAMPLE_ARRAY	0x0020
#define LT_SOUND						0x0030
#define LT_MUSIC_OGG					0x0040
#define LT_MUSIC_MP3					0x0050
#define LT_TEXT							0x0060
#define LT_INPUT_BINDINGS				0x0070
#define LT_MAP							0x0080
#define LT_TRANSFORM_FEEDBACK_BUFFER	0x0090
#define LT_FONT							0x00A0
#define LT_SCRIPT						0x00B0
#define LT_ANIMATION					0x00C0
#define LT_THING						0x00D0
#define LT_FRAMEBUFFER					0x00E0
#define LT_RENDERBUFFER					0x00F0

using namespace std;
using namespace rapidjson;

void Recurse(Document&);
void Recurse(string, Value&);

static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };

// Lump name type/name pairs
pair<string, uint16_t> lumpPairs[] = { {"empty", LT_EMPTY},
									   {"vertexShader", LT_VERTEX_SHADER},
									   {"fragmentShader", LT_FRAGMENT_SHADER},
									   {"program", LT_PROGRAM},
									   {"", LT_EMPTY} };

FILE* fOut;

void Recurse(Document& d)
{
	for (Value::MemberIterator itr = d.MemberBegin();
		itr != d.MemberEnd(); ++itr)
	{
		Recurse(itr->name.GetString(), itr->value);
	}
}

void Recurse(string name, Value& value)
{
	cout << name << " :" << endl;
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	Document dOut;
	Value subLumps(0);

	// Write the name
	fwrite(name.data(), sizeof(char), name.length(), fOut);

	// Add a NULL
	fputc('\0', fOut);

	// Write the lump type
	string type;
	if (!value.HasMember("type"))
	{
		type = "empty";
	}
	else
	{
		type = value["type"].GetString();
	}

	// Find the lump type
	uint16_t contentType = LT_EMPTY;

	int i = 0;
	while (true)
	{
		if (lumpPairs[i].first.length() == 0)
			break;
		if (lumpPairs[i].first == type)
		{
			contentType = lumpPairs[i].second;
			break;
		}
		i++;
	}

	// Write the type
	fwrite(&contentType, sizeof(uint16_t), 1, fOut);

	// Write the metadata
	dOut.CopyFrom(value, dOut.GetAllocator());
	if (dOut.HasMember("subLumps"))
	{
		// Copy the subLumps to the subLumps value
		// Syntax is weird: this basically moves 
		// dOut["subLumps"] to subLumps, while
		// simultaneously setting dOut["subLumps"]
		// to NULL
		dOut["subLumps"] = subLumps;
	}
	dOut.Accept(writer);

	// Check for subLumps
	if (subLumps.IsObject())
	{
		for (Value::MemberIterator itr = subLumps.MemberBegin();
			itr != subLumps.MemberEnd(); ++itr)
		{
			cout << itr->name.GetString() << " :" << endl;
			Recurse(itr->name.GetString(), itr->value);
		}
	}

	string metadata(s.GetString());

	// Write the metadata
	fwrite(metadata.data(), sizeof(char), metadata.length(), fOut);

	// Terminate the metadata
	fputc('\0', fOut);

	// Pop up a level
	fputc('\0', fOut);
}

int main(int argc, char *argv[])
{
	string fileName = "C:\\Users\\Cramer Family\\source\\repos\\Lump2Bytecode\\Debug\\TestLump.json";
	string outName = "C:\\Users\\Cramer Family\\source\\repos\\Lump2Bytecode\\Debug\\TestLump.lmp";
	FILE* f = fopen(fileName.data(), "rb");
	Document d;

	// Determine file size
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);

	char* where = new char[size+1];
	where[size] = '\0';


	// Rewind, read, and load into a string
	rewind(f);
	fread(where, sizeof(char), size, f);
	string contents(where);
	fclose(f);

	// Delete the buffer
	delete[] where;

	// Display the contents
	cout << contents << endl;

	d.Parse(contents.data(), contents.length());

	if (!d.IsObject())
	{
		cout << "Error, invalid JSON file" << endl;
		cin.get();
		return -1;
	}

	// Oopen the output file
	fOut = fopen(outName.data(), "wb");

	if (!fOut)
	{
		cout << "Failed to open output file" << endl;
		cin.get();
		return -1;
	}

	Recurse(d);

	fclose(fOut);

	cin.get();

	return 0;
}