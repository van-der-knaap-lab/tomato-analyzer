# Tomato Analyzer

More information about this project can be found on the [homepage](https://vanderknaaplab.uga.edu/tomato_analyzer.html).

## References

To cite this software, please refer to the following publications:

- [Development of a controlled vocabulary and software application to analyze fruit shape variation in and other plant species. Brewer, M.T., Lang, L., Fujimura, K., Dujmovic, N., Gray, S., and van der Knaap, E. Plant Physiology, 2006, 141, 15-25](https://vanderknaaplab.uga.edu/files/plant_phys_paper.pdf)
- [Tomato Analyzer-Color Test: a new tool for efficient digital phenotyping. Darrigues, A., Hall, J., van der Knaap, E. and Francis, D.M. J. Am. Soc. of Hort. Sci., 2008, 133, 579-586.](https://vanderknaaplab.uga.edu/vdknaap-staging/files/Darrigues_et_al_2008-TA%20_Color_Test-JASHS.pdf)
- [Tomato Fruit Shape Analysis Using Morphometric and Morphology Attributes Implemented in Tomato Analyzer Software Program. Gonzalo, M.J. Brewer, M.T., Anderson, C., Sullivan, D., Gray, S., and van der Knaap, E. J. Am. Soc. of Hort. Sci., 2009, 134, 77-87](https://vanderknaaplab.uga.edu/files/Gonzalo_et_al_2009.pdf)

## Acknowledgements

Tomato Analyzer was developed in collaboration with students from the AMRE program at the College of Wooster. 

**Note:** [@w-bonelli](https://github.com/w-bonelli) did not write and does not maintain this software, only moved it to GitHub. 

## Setup

1. Clone the repository.
2. Download the submodules, if necessary. We rely on rapidjson. The project page for that is found [here](https://github.com/miloyip/rapidjson). You *should* only have to change navigate to `rapidjson/` inside of `tomato-analyzer` and run `git submodule init`, but stuff happens.
3. Download CVIPtools from [here](http://cviptools.ece.siue.edu/downloads.php). At the time of writing, we are building against 5.5d.
4. Download CMake from [here](http://www.cmake.org/). You'll need this to build rapidjson. To build rapidjson:
    1. Open CMake.
    2. The source code should be `path-to-tomato-analyzer/rapidjson`.
    3. You're going to click `Browse Build` and create a new folder in `rapidjson`. Call this folder `build`. 
    4. Select `configure` and then `generate`. We're building against VC100 currently. You might try something else.
5. At this point, you can open the Tomato Analyzer project located inside of `tomato-analyzer`. Do so.
6. Add `rapidjson/include` to the header search path. (Project Properties -> C/C++ -> Additional Include Directories)
7. Add the `CVIPtools/include` directory to the header search path. 
8. Make sure that `CVIPtools/lib` is located inside of the Additional Library Directories. (Project Properties -> Linker -> General -> Additional Link Directories). 
9. Make sure that `CVIPtools/lib` is an additional link dependency. (Project Properties -> Linker -> Input -> Additional Dependencies). 

You should now be able to build.
