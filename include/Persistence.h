#pragma once
#include "FaceDatabase.h"

bool saveFaceDatabase(const FaceDatabase &db, const char* path);
bool loadDatabase(FaceDatabase &db, const char* path);