#include <raytracing/integrator.h>
#include <time.h>
#include <ctime>


Integrator::Integrator():
    max_depth(5),
    mersenne_generator(time(0)),
    unif_distribution(0.f,1.f)
{
    scene = NULL;
    intersection_engine = NULL;
}



glm::vec3 ComponentMult(const glm::vec3 &a, const glm::vec3 &b)
{
    return glm::vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}


void Integrator::SetDepth(unsigned int depth)
{
    max_depth = depth;
}


BxDFType Integrator::GetWeightedType(const Intersection &isx, float rand)
{
    int index = 0;
    float runningTotal = 0.f;

    for (BxDF* b : isx.object_hit->material->bxdfs) {
        float w = isx.object_hit->material->bxdf_weights[index];
        // using index/2 because the actual weights are separated by 0s in the list
        if(rand < w + runningTotal) {
            return isx.object_hit->material->bxdfs[index/2]->type;
        }
        else {
            runningTotal += w;
            index +=2;
        }
    }
    return isx.object_hit->material->bxdfs[0]->type;
}


glm::vec3 Integrator::DirectLighting(Ray r, const Intersection &objectIsc, int samples)
{

    glm::vec3 Ld(0.f), Li(0.f);  // cumulative color
    BxDFType type;  // the bxdf type actually sampled is stored here
    BxDFType flags = BSDF_ALL; // flag for weighted material sampling
    float lightPDF, brdfPDF;
    float absdot;
    float rand1, rand2;
    glm::vec3 brdf;
    float f, g, weight;
    glm::vec3 wi;
    Ray wiRay;
    Intersection visibilityTester;

    for (int i = 0; i < samples; i++){

        // If weighted material, select a bxdf based on weights
        if (objectIsc.object_hit->material->is_weighted) {
            float r = unif_distribution(mersenne_generator);
            flags = GetWeightedType(objectIsc, r);
        }

        // Pick a random light source
        int index = rand() % scene->lights.length();
        Geometry *light = scene->lights.at(index);

        // Sample the light
        rand1 = unif_distribution(mersenne_generator);
        rand2 = unif_distribution(mersenne_generator);
        Intersection lightIsc = light->GetSampledIntersection(rand1, rand2, objectIsc.normal);
        wiRay = Ray(objectIsc.point + glm::vec3((lightIsc.point - objectIsc.point) * (float)0.0001), lightIsc.point - objectIsc.point);
        lightPDF = light->RayPDF(lightIsc, wiRay);

        if (lightPDF > 0.f) {
            visibilityTester = intersection_engine->GetIntersection(wiRay);

            if (visibilityTester.object_hit == lightIsc.object_hit) {
                brdf = objectIsc.object_hit->material->EvaluateScatteredEnergy(objectIsc, -r.direction, wiRay.direction, flags);
                brdfPDF = objectIsc.object_hit->material->PDF(objectIsc, -r.direction, wiRay.direction);
                Li = light->material->EvaluateScatteredEnergy(lightIsc, glm::vec3(0.f), -wiRay.direction);
                absdot = fabs(glm::dot(wiRay.direction, objectIsc.normal));

                // MIS
                f = samples * lightPDF;
                g = samples * brdfPDF;
                weight = (f*f) / (f*f + g*g);

                // Add the direct lighting for this sample
                Ld += brdf * Li * (absdot * weight / lightPDF);
            }
        }

        // Sample the brdf
        rand1 = unif_distribution(mersenne_generator);
        rand2 = unif_distribution(mersenne_generator);
        brdf = objectIsc.object_hit->material->SampleAndEvaluateScatteredEnergy(objectIsc, -r.direction, wi, brdfPDF, rand1, rand2, type, flags);

        if (brdfPDF > 0.f) {

            wiRay = Ray(objectIsc.point + wi * (float)0.0001, wi);
            visibilityTester = intersection_engine->GetIntersection(wiRay);

            if (visibilityTester.object_hit) {

                if (visibilityTester.object_hit == light) {

                    if (type & BSDF_SPECULAR) {
                        weight = 1.f;
                    } else {

                        // Get light pdf
                        lightPDF = light->RayPDF(visibilityTester, wiRay);
                        if (lightPDF == 0.f) continue;

                        // MIS
                        f = samples * brdfPDF;
                        g = samples * lightPDF;
                        weight = (f*f) / (f*f + g*g);
                    }
                    Li = light->material->EvaluateScatteredEnergy(visibilityTester, glm::vec3(0.f), -wiRay.direction);
                    absdot = fabs(glm::dot(wiRay.direction, objectIsc.normal));

                    // Add the direct lighting for this sample
                    Ld += brdf * Li * absdot * weight / brdfPDF;
                }
            }
        }
    }
    return Ld / (float) samples;
}

glm::vec3 Integrator::TraceRay(Ray r, unsigned int depth)
{
    int samples = 50;

    Intersection objectIsc = intersection_engine->GetIntersection(r);
    if (!objectIsc.object_hit) return glm::vec3(0.f);
    if (objectIsc.object_hit->material->is_light_source) return glm::vec3(1.f);

    return DirectLighting(r, objectIsc, samples);
}


glm::vec3 Integrator::TraceRayTransport(Ray r, unsigned int depth)
{
    int overallSamples = 65;  // # of initial brdf samples taken at ray's intersection with scene
    int dlSamples = 10;  // # of direct lighting samples taken at each ray bounce
    // The initial intersection gets more samples (this happens at the end)

    glm::vec3 Ltotal(0.f); // Keeps track of light across overall samples

    // Intersect the ray with the scene
    Intersection objectIsc = intersection_engine->GetIntersection(r);
    if (!objectIsc.object_hit) return glm::vec3(0.f);
    if (objectIsc.object_hit->material->is_light_source) return glm::vec3(1.f);

    for (int i = 0; i < overallSamples; i++) {
        glm::vec3 L(0.f), T(1.f);  // throughput and lighting
        Intersection tempIsc = objectIsc;
        Ray tempRay = r;
        bool specular = false;
        bool terminate = false;

        for (int bounces = 0; ;bounces++) {

            if (bounces == depth) break;

            // sample new direction
            BxDFType flag, type;
            float pdf;
            glm::vec3 wi_sampled;
            float rand1 = unif_distribution(mersenne_generator);
            float rand2 = unif_distribution(mersenne_generator);
            glm::vec3 brdf = tempIsc.object_hit->material->SampleAndEvaluateScatteredEnergy(tempIsc, -tempRay.direction, wi_sampled, pdf, rand1, rand2, type, flag);
            if (pdf == 0.f) break;

            if (type & BSDF_SPECULAR) {
                specular = true;
            } else specular = false;

            // adjust throughput based on sample
            float absdot = fabs(glm::dot(wi_sampled, tempIsc.normal));
            T *= brdf * absdot / pdf;

            if (specular) L += T * glm::vec3(1.f);


            // russian roulette
            if (bounces > 2) {
                float a = 0.212671f * T.x + 0.715160f * T.y + 0.072169f * T.z;
                float prob = std::min(.5f, a);
                float rand = unif_distribution(mersenne_generator);
                if (prob < rand) break;
            }

            // update ray and intersection
            tempRay = Ray(tempIsc.point + wi_sampled * 0.0001f, wi_sampled);
            tempIsc = intersection_engine->GetIntersection(tempRay);

            // test new ray against scene
            if (tempIsc.object_hit) { // hit something
                if (!(tempIsc.object_hit->material->is_light_source)) { // not a light
                    L += T * DirectLighting(tempRay, tempIsc, dlSamples);
                } else { // hit a light
                    terminate = true;
                    if (specular) {
                        if (glm::dot(tempIsc.normal, -tempRay.direction) > 0.f) {
                            L += T * glm::vec3(1.f);
                        }
                    }
                }
            } else terminate = true;

            if (terminate) break;
        } // for bounce
        Ltotal += L;
    } // for sample
    dlSamples = 200;
    return Ltotal / (float) overallSamples + DirectLighting(r, objectIsc, dlSamples);
}

void Integrator::PhotonMapper(Ray r, unsigned int depth)
{
    std::vector<photon_t> directPhotons, indirectPhotons, causticPhotons;  // vectors for storing our photon samples
    int pathsInitiated = 0;  // maintains count of how many rays were shot from light sources
    int maxPaths = 1000;  // limit for total photon rays shot from lights
    int maxBounces = 5;  // limit for number of bounces down a particular path


    for (;;pathsInitiated++)
    {
        // Sample a light source, a point on that light, and a ray direction originating at that point.
        int index = rand() % scene->lights.length();  // eventually update this to sample based on intensity
        Geometry *sampledLight = scene->lights.at(index);
        float rand1 = unif_distribution(mersenne_generator);
        float rand2 = unif_distribution(mersenne_generator);
        glm::vec3 randNormal(unif_distribution(mersenne_generator),unif_distribution(mersenne_generator),unif_distribution(mersenne_generator));
        Intersection sampledIsx = sampledLight->GetSampledIntersection(rand1, rand2, randNormal);
        rand1 = unif_distribution(mersenne_generator);
        rand2 = unif_distribution(mersenne_generator);
        glm::vec3 sampledDirection = sampledLight->SamplePhotonDirection(sampledIsx, rand1, rand2);


        Ray photonRay(sampledIsx.point, sampledDirection);
        bool purelySpecular = true;
        for (int bounces = 0; ;bounces++)
        {
            if (bounces > maxBounces) break;

            // Intersect the photon ray with the scene, breaking if we missed all non-emissive geometry
            Intersection photonIsx = intersection_engine->GetIntersection(photonRay);
            if (!photonIsx.object_hit) break;
            if (photonIsx.object_hit->material->is_light_source) break;

            //if (!photonIsx.object_hit->material->bxdfs & BSDF_SPECULAR) // TODO fix this to correctly check if material has non-specular bxdf
            {
                // Create a single photon
                photon_t currentPhoton(photonIsx, -photonRay.direction);

                if (purelySpecular && bounces > 0) {
                    causticPhotons.push_back(currentPhoton);
                }
                else {
                    bounces == 0 ? directPhotons.push_back(currentPhoton) : indirectPhotons.push_back(currentPhoton);
                }

                // TODO handle radiance photons
            }

            // Sample next bounced direction
            glm::vec3 nextDirection;
            float pdf;
            BxDFType type;
            glm::vec3 brdf = photonIsx.object_hit->material->
                    SampleAndEvaluateScatteredEnergy(
                    photonIsx, -photonRay.direction, nextDirection, pdf,
                    unif_distribution(mersenne_generator), unif_distribution(mersenne_generator), type);

            photonRay = Ray(photonIsx.point, nextDirection);
        }


    }

}
