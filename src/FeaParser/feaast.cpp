/*
 * Copyright (c) 2015-2020 Amine Anane. http: //digitalkhatt/license
 * This file is part of DigitalKhatt.
 *
 * DigitalKhatt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * DigitalKhatt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.

 * You should have received a copy of the GNU Affero General Public License
 * along with DigitalKhatt. If not, see
 * <https: //www.gnu.org/licenses />.
*/

#include "feaast.h"
#include "Subtable.h"

namespace feayy {

	void LookupFlag::accept(Visitor& v) { v.accept(*this); }
	void LookupDefinition::accept(Visitor& v) { v.accept(*this); }
	void FeatureReference::accept(Visitor& v) { v.accept(*this); }
	void LookupReference::accept(Visitor& v) { v.accept(*this); }
	void ChainingContextualRule::accept(Visitor& v) { v.accept(*this); }
	void SingleAdjustmentRule::accept(Visitor& v) { v.accept(*this); }
	void CursiveRule::accept(Visitor& v) { v.accept(*this); }
	void Mark2BaseRule::accept(Visitor& v) { v.accept(*this); }
	void SingleSubstituionRule::accept(Visitor& v) { v.accept(*this); }
	void LookupStatement::accept(Visitor& v) { v.accept(*this); }
	void FeatureDefenition::accept(Visitor& v) { v.accept(*this); }
	void ClassDefinition::accept(Visitor& v) { v.accept(*this); }
	void MarkedGlyphSetRegExp::accept(Visitor& v) { v.accept(*this); }
	void LigatureSubstitutionRule::accept(Visitor& v) { v.accept(*this); }



	void FeaContext::populateFeatures() {

		LookupDefinitionVisitor feaVisitor{ otlayout , *this };

		for (auto feature : features)
		{
			FeatureDefenition* featureDefinition = feature.second;

			featureDefinition->accept(feaVisitor);
		}

	}

	void FeaContext::useLookup(QString lookupName) {		

		auto liter = lookups.find(lookupName.toStdString());

		if (liter == lookups.end())
			return;

		LookupDefinition* lookupDefinition = liter->second;

		LookupDefinitionVisitor lookupVisitor{ otlayout , *this };

		lookupDefinition->accept(lookupVisitor);
	}

    LookupDefinitionVisitor::LookupDefinitionVisitor(OtLayout* otlayout, FeaContext& context) :
        lookup{ nullptr },otlayout{ otlayout }, refLookups{ nullptr },context{ context }{
	}

	void LookupDefinitionVisitor::accept(LookupFlag& flag) {

		lookup->flags = flag.getFlag();

		auto glyphSet = flag.getMarkFilteringSet();

		if (glyphSet != nullptr) {
            lookup->markGlyphSetIndex = otlayout->addMarkSet(glyphSet->getCodes(otlayout).values());
		}

	}
	void LookupDefinitionVisitor::accept(LookupDefinition& lookupDefinition) {

		QSet<QString>* old_refLookups = this->refLookups;
		Lookup* old_lookup = this->lookup;
		int old_nextautolookup = this->nextautolookup;

		QSet<QString> localrefLookups;

		this->refLookups = &localrefLookups;
		this->lookup = new Lookup(otlayout);

		lookup->name = QString::fromStdString(lookupDefinition.getName());

		for (auto stmt : lookupDefinition.getStmts()) {
			stmt->accept(*this);
		}

		if (lookup->subtables.count() != 0) {
			otlayout->addLookup(lookup);
		}

		for (auto refLookupName : localrefLookups) {
			auto lookupsIndex = otlayout->lookupsIndexByName.value(refLookupName, -1);
			if (lookupsIndex == -1) {
				auto liter = context.lookups.find(refLookupName.toStdString());

				if (liter == context.lookups.end())
					continue;

				LookupDefinition* lookupDefinition = liter->second;

				lookupDefinition->accept(*this);
			}
			else {
				if (!currentFeature.empty()) {
					Lookup* ll = otlayout->lookups[lookupsIndex];
					if (!ll->feature.isEmpty()) {
						otlayout->allFeatures[QString::fromStdString(currentFeature)].insert(ll);
					}
				}
			}
		}

		this->refLookups = old_refLookups;
		this->lookup = old_lookup;
		this->nextautolookup = old_nextautolookup;


	}

	void LookupDefinitionVisitor::accept(FeatureReference& featureReference) {
		lookup->feature = featureReference.featureName;
	}

	void LookupDefinitionVisitor::accept(SingleAdjustmentRule& singleRule) {


		if (lookup->type == Lookup::none) {
			lookup->type = Lookup::singleadjustment;
		}
		else if (lookup->type != Lookup::singleadjustment) {
			throw "Lookup with different subtable type";
		}

		int format = 2;

		if (singleRule.color) {
			format = 3;
		}

		int subtableName = lookup->subtables.length() + 1;
		SingleAdjustmentSubtable* newsubtable = nullptr;
		if (lookup->subtables.length() > 0) {
			newsubtable = static_cast<SingleAdjustmentSubtable*>(lookup->subtables.last());
		}

		if (newsubtable == nullptr || newsubtable->format != format) {
			newsubtable = new SingleAdjustmentSubtable(lookup, format);
			newsubtable->name = QString("subtable%1").arg(subtableName);
			lookup->subtables.append(newsubtable);
		}

		auto  unicodes = singleRule.glyphset->getCodes(otlayout);

		for (auto unicode : unicodes) {

			newsubtable->singlePos[unicode] = singleRule.valueRecord;
		}



	}

	void LookupDefinitionVisitor::accept(CursiveRule& cursiveRule) {
		if (lookup->type == Lookup::none) {
			lookup->type = Lookup::cursive;
		}
		else if (lookup->type != Lookup::cursive) {
			throw "Lookup with different subtable type";
		}

		int subtableName = lookup->subtables.length() + 1;
		CursiveSubtable* newsubtable = nullptr;
		if (lookup->subtables.length() > 0) {
			newsubtable = static_cast<CursiveSubtable*>(lookup->subtables.last());
		}

		if (newsubtable == nullptr) {
			newsubtable = new CursiveSubtable(lookup);
			newsubtable->name = QString("subtable%1").arg(subtableName);
			lookup->subtables.append(newsubtable);
		}

		CursiveSubtable::EntryExit value;

		if (cursiveRule.entryAnchor->anchortype() == AnchorType::FormatA) {
			auto anchor = static_cast<AnchorFormatA*>(cursiveRule.entryAnchor);
			value.entry = QPoint(anchor->x, anchor->y);
        }else if (cursiveRule.entryAnchor->anchortype() == AnchorType::Name) {
            auto anchor = static_cast<AnchorName*>(cursiveRule.entryAnchor);
            value.entryName = QString::fromStdString(*anchor->name);
        }else if (cursiveRule.entryAnchor->anchortype() == AnchorType::Function) {
            auto functionAnchor = static_cast<AnchorFunction*>(cursiveRule.entryAnchor);
            value.entryFunction = otlayout->getanchorCalcFunctions(QString::fromStdString(*functionAnchor->name), newsubtable);
        }

		if (cursiveRule.exitAnchor->anchortype() == AnchorType::FormatA) {
			auto anchor = static_cast<AnchorFormatA*>(cursiveRule.exitAnchor);
			value.exit = QPoint(anchor->x, anchor->y);
        }else if (cursiveRule.exitAnchor->anchortype() == AnchorType::Name) {
            auto anchor = static_cast<AnchorName*>(cursiveRule.exitAnchor);
            value.exitName = QString::fromStdString(*anchor->name);
        }else if (cursiveRule.exitAnchor->anchortype() == AnchorType::Function) {
            auto functionAnchor = static_cast<AnchorFunction*>(cursiveRule.exitAnchor);
            value.exitFunction = otlayout->getanchorCalcFunctions(QString::fromStdString(*functionAnchor->name), newsubtable);
        }

		auto codes = cursiveRule.glyphset->getCodes(otlayout);

		for (auto code : codes) {
			newsubtable->anchors[code] = value;
		}
	}

	void LookupDefinitionVisitor::accept(Mark2BaseRule& mark2BaseRule) {
		if (lookup->type == Lookup::none) {
			lookup->type = mark2BaseRule.type;
		}
		else if (lookup->type != mark2BaseRule.type) {
			throw "Lookup with different subtable type";
		}

		int subtableName = lookup->subtables.length() + 1;
		MarkBaseSubtable* newsubtable = new MarkBaseSubtable(lookup);
		newsubtable->name = QString("subtable%1").arg(subtableName);
		lookup->subtables.append(newsubtable);

		newsubtable->sortedBaseCodes = mark2BaseRule.baseGlyphSet->getSortedCodes(otlayout);

		for (auto mark2baseclass : *mark2BaseRule.mark2baseclasses) {
			QString className = QString::fromStdString(*mark2baseclass->className);

			MarkBaseSubtable::MarkClass newclass;

			newclass.markCodes = mark2baseclass->glyphset->getCodes(otlayout);

			if (mark2baseclass->baseAnchor->anchortype() == AnchorType::Function) {
				auto functionAnchor = static_cast<AnchorFunction*>(mark2baseclass->baseAnchor);
				newclass.basefunction = otlayout->getanchorCalcFunctions(QString::fromStdString(*functionAnchor->name), newsubtable);
			}
			else if (mark2baseclass->baseAnchor->anchortype() == AnchorType::FormatA) {
				auto formaAAnchor = static_cast<AnchorFormatA*>(mark2baseclass->baseAnchor);
				auto anchor = QPoint{ formaAAnchor->x,formaAAnchor->y };
				for (auto code : newsubtable->sortedBaseCodes) {
					auto glyphName = otlayout->glyphNamePerCode[code];
					newclass.baseanchors[glyphName] = anchor;
				}
			}

			if (mark2baseclass->markAnchor->anchortype() == AnchorType::Function) {
				auto functionAnchor = static_cast<AnchorFunction*>(mark2baseclass->markAnchor);
				newclass.markfunction = otlayout->getanchorCalcFunctions(QString::fromStdString(*functionAnchor->name), newsubtable);
			}
			else if (mark2baseclass->markAnchor->anchortype() == AnchorType::FormatA) {
				auto formaAAnchor = static_cast<AnchorFormatA*>(mark2baseclass->markAnchor);
				auto anchor = QPoint{ formaAAnchor->x,formaAAnchor->y };
				for (auto code : newclass.markCodes) {
					auto markName = otlayout->glyphNamePerCode[code];
					newclass.markanchors[markName] = anchor;
				}
			}

			newsubtable->classes[className] = newclass;

		}
	}

	void LookupDefinitionVisitor::accept(SingleSubstituionRule& singleRule) {


		if (lookup->type == Lookup::none) {
			lookup->type = Lookup::single;
		}
		else if (lookup->type != Lookup::single) {
			throw "Lookup with different subtable type";
		}

		int subtableName = lookup->subtables.length() + 1;
		SingleSubtable* newsubtable = nullptr;
		if (lookup->subtables.length() > 0) {
			newsubtable = static_cast<SingleSubtable*>(lookup->subtables.last());
		}

		if (newsubtable == nullptr || newsubtable->format != singleRule.format) {

			if (singleRule.format == 10) {
				newsubtable = new SingleSubtableWithExpansion(lookup);
			}
			else if (singleRule.format == 11) {
				newsubtable = new SingleSubtableWithTatweel(lookup);
			}
			else {
				newsubtable = new SingleSubtable(lookup, singleRule.format);
			}

			newsubtable->name = QString("subtable%1").arg(subtableName);
			lookup->subtables.append(newsubtable);
		}

		if (singleRule.format != 11) {

			auto  firstunicodes = singleRule.firstglyph->getCodes(otlayout);

			if (firstunicodes.size() != 1) {
				throw "Single subtitution : first glyph different to 1 matching";
			}

			auto firstunicode = *firstunicodes.begin();

			auto  secondtunicodes = singleRule.secondglyph->getCodes(otlayout);

			if (secondtunicodes.size() != 1) {
				throw "Single subtitution : second glyph different to 1 matching";
			}


			auto secondunicode = *secondtunicodes.begin();

			newsubtable->subst[firstunicode] = secondunicode;

			if (singleRule.format == 10) {
				((SingleSubtableWithExpansion*)newsubtable)->expansion[firstunicode] = singleRule.expansion;
				((SingleSubtableWithExpansion*)newsubtable)->expansion[firstunicode].startEndLig = singleRule.startEndLig;
			}
		}
		else if (singleRule.format == 11) {

			auto  firstunicodes = singleRule.firstGlyphSet->getCodes(otlayout);

			SingleSubtableWithTatweel* subtable = (SingleSubtableWithTatweel*)newsubtable;

			for (auto code : firstunicodes) {
				subtable->expansion[code] = singleRule.expansion;
			}


		}

	}
	void LookupDefinitionVisitor::accept(ChainingContextualRule& contextualRule) {

		if (lookup->type == Lookup::none) {
			lookup->type = contextualRule.type;
		}
		else if (lookup->type != contextualRule.type) {
			throw "Lookup with different subtable type";
		}

		QVector<QVector<GlyphSet*>> backtrack;

		if (contextualRule.backtrack) {
			backtrack = contextualRule.backtrack->getSequences();
		}
		else {
			backtrack = { {} };
		}

		QVector<QVector<QSet<quint16>>> compiledbacktrack;

		for (auto seq : backtrack) {
			QVector<QSet<quint16>> compiledseq;
			for (auto glyphset : seq) {
				auto set = glyphset->getCodes(otlayout);
				if (!set.isEmpty()) {
					compiledseq.append(set);
				}
				else {
					throw "Glyphset is empty";
				}
			}
			compiledbacktrack.append(compiledseq);
		}

		QVector<QVector<GlyphSet*>> lookahead;

		if (contextualRule.lookahead) {
			lookahead = contextualRule.lookahead->getSequences();
		}
		else {
			lookahead = { {} };
		}

		QVector<QVector<QSet<quint16>>> compiledlookahead;

		for (auto seq : lookahead) {
			QVector<QSet<quint16>> compiledseq;
			for (auto glyphset : seq) {
				auto set = glyphset->getCodes(otlayout);
				if (!set.isEmpty()) {
					compiledseq.append(set);
				}
				else {
					throw "Glyphset is empty";
				}
			}
			compiledlookahead.append(compiledseq);
		}

		QVector<QVector<QPair<GlyphSet*, QString>>> compiledinputs = { {} };

		LookupDefinition* lastautolookup{ nullptr };
		InlineType lastInlineType{ InlineType::None };

		for (auto value : *contextualRule.input) {
			//value->accept(*this);
			if (value->inlineType != InlineType::None) {
				if (lastInlineType == value->inlineType && lastInlineType == InlineType::CursivePos) {
					lastautolookup->getStmts().push_back(value->stmt);
					std::string named = lastautolookup->getName();
					value->lookupName = new std::string(named);
				}
				else {
					auto stmts = new vector<LookupStatement*>();
					stmts->push_back(value->stmt);

					QString name = QString("%1_auto%2").arg(lookup->name).arg(nextautolookup++);

					std::string named = name.toStdString();

					value->lookupName = new std::string(named);
					lastautolookup = new LookupDefinition(value->lookupName, stmts);
					context.lookups[*value->lookupName] = lastautolookup;
				}
			}
			lastInlineType = value->inlineType;
			auto input = value->regexp->getSequences();
			QString lookupName;
			if (value->lookupName) {
				lookupName = QString::fromStdString(*value->lookupName);
			}

			QVector<QVector<QPair<GlyphSet*, QString>>> result;
			for (auto ii : input) {

				auto temp = compiledinputs;

				for (auto& compiledinput : temp) {
					for (auto jj : ii) {
						compiledinput.append(QPair<GlyphSet*, QString>{ jj, lookupName });
					}
				}

				result.append(temp);

			}

			compiledinputs = result;

		}

		for (auto backtrack : compiledbacktrack) {
			for (auto lookahead : compiledlookahead) {
				for (auto input : compiledinputs) {
					ChainingSubtable* newsubtable = new ChainingSubtable(lookup);
					lookup->subtables.append(newsubtable);

					newsubtable->compiledRule.backtrack = backtrack;

					for (auto pair : input) {
						auto set = pair.first->getCodes(otlayout);
						if (!set.isEmpty()) {
							if (!pair.second.isEmpty()) {
								QString lookupName = pair.second;
								refLookups->insert(lookupName);
								newsubtable->compiledRule.lookupRecords.append({ quint16(newsubtable->compiledRule.input.size()),lookupName });
							}
							newsubtable->compiledRule.input.append(set);
						}
						else {
							throw "empty set in lookup " + lookup->name;
						}
					}

					newsubtable->compiledRule.lookahead = lookahead;
				}

			}
		}
	}

	void LookupDefinitionVisitor::accept(ClassDefinition& classDef) {
		QSet<QString> set;

		for (auto glyph : classDef.components->getCodes(otlayout)) {
			set.insert(otlayout->glyphNamePerCode[glyph]);
		}
		otlayout->addClass(classDef.name, set);
	}

	void LookupDefinitionVisitor::accept(MarkedGlyphSetRegExp& markedGlyphSetRegExp) {
		if (markedGlyphSetRegExp.inlineType != InlineType::None) {
			auto stmts = new vector<LookupStatement*>();
			stmts->push_back(markedGlyphSetRegExp.stmt);

			QString name = QString("%1_auto%2").arg(lookup->name).arg(nextautolookup++);

			markedGlyphSetRegExp.lookupName = new std::string(name.toStdString());
			auto lookup = new LookupDefinition(markedGlyphSetRegExp.lookupName, stmts);
			context.lookups[*markedGlyphSetRegExp.lookupName] = lookup;

		}
	}

	void LookupDefinitionVisitor::accept(LookupStatement&) {
	}

	void LookupDefinitionVisitor::accept(LookupReference& lookupReference) {

		auto lookupsIndex = otlayout->lookupsIndexByName.value(lookupReference.lookupName, -1);

		if (lookupsIndex == -1) {		

			auto liter = context.lookups.find(lookupReference.lookupName.toStdString());

			if (liter == context.lookups.end()) {
				otlayout->parseCppJsonLookup(lookupReference.lookupName, *context.json);
				return;
			}

			LookupDefinition* lookupDefinition = liter->second;

			lookupDefinition->accept(*this);
		}
		else {
			if (!currentFeature.empty()) {
				Lookup* ll = otlayout->lookups[lookupsIndex];
				if (!ll->feature.isEmpty()) {
					otlayout->allFeatures[QString::fromStdString(currentFeature)].insert(ll);
				}
			}
		}
	}
	void LookupDefinitionVisitor::accept(FeatureDefenition& featureDefenition) {

		this->currentFeature = featureDefenition.getName();

		for (auto stmt : featureDefenition.getStmts()) {
			stmt->accept(*this);
		}

		this->currentFeature.clear();
	}
	void LookupDefinitionVisitor::accept(LigatureSubstitutionRule& ligatureSubstitutionRule) {

		if (lookup->type == Lookup::none) {
			lookup->type = Lookup::ligature;
		}
		else if (lookup->type != Lookup::ligature) {
			throw "Lookup with different subtable type";
		}

		int subtableName = lookup->subtables.length() + 1;
		LigatureSubtable* newsubtable = nullptr;
		if (lookup->subtables.length() > 0) {
			newsubtable = static_cast<LigatureSubtable*>(lookup->subtables.last());
		}

		if (newsubtable == nullptr || newsubtable->format != ligatureSubstitutionRule.format) {			

			newsubtable = new LigatureSubtable(lookup);

			newsubtable->name = QString("subtable%1").arg(subtableName);
			lookup->subtables.append(newsubtable);
		}

		auto  firstunicodes = ligatureSubstitutionRule.ligature->getCodes(otlayout);

		if (firstunicodes.size() != 1) {
			throw "ligature subtitution : ligature glyph different to 1 matching";
		}

		LigatureSubtable::Ligature ligStruct;

		ligStruct.ligatureGlyph = *firstunicodes.begin();

		

		for (auto glyph : *ligatureSubstitutionRule.sequence) {
			auto unicodes = glyph->getCodes(otlayout);
			if (unicodes.size() != 1) {
				throw "ligature subtitution : glyph different to 1 matching";
			}

			ligStruct.componentGlyphIDs.append(*unicodes.begin());			
		}

		newsubtable->ligatures.append(ligStruct);
	}


}
