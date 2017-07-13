/*
 * Neuromapp - Reac.h, Copyright (c), 2015,
 * Francesco Casalegno - Swiss Federal Institute of technology in Lausanne,
 * francesco.casalegno@epfl.ch,
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/readi/Model.h
 * \brief Reactions, Diffusions, and Species Model for Readi Miniapp
 */

#ifndef MAPP_READI_MODEL_
#define MAPP_READI_MODEL_


namespace readi {


template <class IntType, class FloatType>
class Model {
public:

    // return number of species
    inline FloatType get_n_species() const {
        return n_species_;
    }


    // return max diffusion coefficient
    inline FloatType get_max_diff() const {
        return *std::max_element(diffusion_coefficients_.begin(), diffusion_coefficients_.end());
    }


    // access diffusion constant of s-th species
    inline FloatType& diffusion_coeff(IntType s) {
        assert(s>=0 && s<n_species_);
        return diffusion_coefficients_[s];
    }
    inline FloatType diffusion_coeff(IntType s) const {
        assert(s>=0 && s<n_species_);
        return diffusion_coefficients_[s];
    }


    // read from file the model data (species names + reactions + diffusions)
    void read_from_file(std::string const& filename_model) {
        std::ifstream f;

        try {
            f.open(filename_model);
            std::string discard;

            // --- READ SPECIES NAMES ---
            f >> discard >> n_species_;                 // read how many species
            species_names_.resize(n_species_);
            diffusion_coefficients_.resize(n_species_);
            std::getline(f, discard);                   // skip '\n'
            std::getline(f, discard);                   // skip description line
            //IntType tot_mol_per_spec;
            for (IntType i=0; i<n_species_; ++i) {
                f >> species_names_[i] >> discard;      // read species name and skip total count
            }
            std::getline(f, discard);                   // skip '\n'

            // --- READ REACTIONS ---
            std::getline(f, discard);                   // skip empty line
            f >> discard >> n_reactions_;               // read how many reactions
            std::getline(f, discard);                   // skip '\n'
            std::getline(f, discard);                   // skip description
            reaction_coefficients_.resize(n_reactions_);
            for (IntType i=0; i<n_reactions_; ++i) {
                std::getline(f, discard);               // read reaction lhs, rhs, and coeff
                std::cout << "+++ reac: " << discard << std::endl;
                std::vector<std::string> lhs;
                std::vector<std::string> rhs;
                double k_reac;
                std::stringstream sstr(discard);
                std::string buf;
                while (sstr >> buf) {
                    if (buf == "-") break;
                    lhs.push_back(buf);
                }
                while (sstr >> buf) {
                    if (buf == "-") break;
                    rhs.push_back(buf);
                }
                sstr >> k_reac;
                reactions_.emplace_back(species_names_, lhs, rhs, k_reac);  // create new reaction with such lhs, rhs, and coeff
            }

            // --- READ DIFFUSIONS ---
            std::getline(f, discard);                   // skip empty line
            std::getline(f, discard);                   // skip description line
            for (IntType s=0; s<n_species_; ++s) {
                f >> discard >> diffusion_coeff(s);     // read coefficients of diffusion
            }

        }
        catch(const std::exception& ex) {
            f.close();
            throw;
        }

        // assert diffusion coefficients are correctely read
        for (IntType i=0; i<n_species_; ++i)
            std::cout << "+++ diff: " << species_names_[i] << " - " << diffusion_coefficients_[i] << "\n";
    }




private:
    IntType n_species_;
    IntType n_reactions_;
    std::vector<std::string> species_names_;
    std::vector<FloatType> diffusion_coefficients_;
    std::vector<FloatType> reaction_coefficients_;
    std::vector< readi::Reac<IntType,FloatType> > reactions_;
};



} // nampespace readi

#endif// MAPP_READI_MODEL_
