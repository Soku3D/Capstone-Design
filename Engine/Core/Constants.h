#pragma once

namespace soku {
	struct BasicVSConstantData {
		Matrix model;
		Matrix view;
		Matrix projection;
		Matrix invTranspose;
	};
	struct CubeVSConstantData {
		Matrix view;
		Matrix projection;
	};
	struct NormalGSConstantData {
		float length = 0.2f;
		Vector3 dummy;
	};
	struct Light {
		Vector3 lightPos = Vector3(0.f, 0.f, -2.f);
		float fallOffStart = 0.0f;
		Vector3 lightStrength = Vector3(1.f);
		float fallOffEnd = 10.0f;
		Vector3 lightDir = Vector3(0.f, 0.f, 1.f);
		float spotPower = 1.0f;
	};
	struct Material {
		Vector3 ambient = Vector3(0.2f);
		float shininess = 1.f;
		Vector3 diffuse = Vector3(0.5f);
		float dummy1 = 0.f;
		Vector3 specular = Vector3(1.f);
		float dummy2 = 0.f;
		Vector3 fresnelR0 = Vector3(1.f, 1.f, 1.f);
		float dummy3 = 0.f;
	};
	struct BasicPSConstantData {
		Vector3 eyeWorld = Vector3(0.0f, 0.15f, 0.0f);
		bool useTexture = false;
		Light lights[3];
		Material material;
	};
	struct BillboardPointConstantData {
		Matrix model;
		Matrix view;
		Matrix projection;
		float dx;
		Vector3 eyeWorld;
		float time = 0.f;
		Vector3 dummy;
	};
}