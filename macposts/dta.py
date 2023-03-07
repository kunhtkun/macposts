"""Dynamic Traffic Assignment (DTA).

This module contains two classes for accessing the dynamic traffic assignment
functionalities in macposts. One is for single class DTA and the other is for
biclass DTA.

Note that this module is supposed to serve as an intermediate module between
the core `libmacposts' and the user.

"""

import numpy as np
import _macposts_ext as _ext


# XXX: I would like to use a common base class instead.
class _CommonMixin:
    """Mixin class for common methods of both Dta and Mcdta."""

    @classmethod
    def from_files(cls, directory):
        """Create an instance of *cls* with files in *directory*."""
        obj = cls()
        obj.initialize(str(directory))
        return obj

    def _get_ccs(self, link_func, links):
        if links is None:
            links = self.registered_links
        else:
            try:
                _ = iter(links)
            except TypeError:
                links = [links]
        ccs = np.hstack([link_func(link) for link in links])
        ticks = ccs[:, 0].astype(np.int_)
        ccs = ccs[:, 1::2]
        return ticks, ccs


class Dta(_CommonMixin, _ext.Dta):
    """Single class DTA."""

    def get_in_ccs(self, links=None):
        """Get the incoming cumulative curves for registered links.

        Required arguments *links* should be an iterable of link IDs and
        specify the desired links for which the cumulative curves will be
        retrieved. It could also be None, in which case all registered links
        will be used. For backward compatibility, if *links* is not iterable,
        it will be treated as a list of one element. However, that is not
        recommended.

        Return a tuple of (TICKS, CUMULATIVE-CURVES), and both elements are
        Numpy arrays.

        """
        return self._get_ccs(self.get_link_in_cc, links)

    def get_out_ccs(self, links=None):
        """Get the outgoing cumulative curves for registered links.

        Required arguments *links* should be an iterable of link IDs and
        specify the desired links for which the cumulative curves will be
        retrieved. It could also be None, in which case all registered links
        will be used. For backward compatibility, if *links* is not iterable,
        it will be treated as a list of one element. However, that is not
        recommended.

        Return a tuple of (TICKS, CUMULATIVE-CURVES), and both elements are
        Numpy arrays.

        """
        return self._get_ccs(self.get_link_out_cc, links)


class Mcdta(_CommonMixin, _ext.Mcdta):
    """Biclass DTA."""

    def get_car_in_ccs(self, links=None):
        """Get the incoming car cumulative curves for registered links.

        Required arguments *links* should be an iterable of link IDs and
        specify the desired links for which the cumulative curves will be
        retrieved. It could also be None, in which case all registered links
        will be used. For backward compatibility, if *links* is not iterable,
        it will be treated as a list of one element. However, that is not
        recommended.

        Return a tuple of (TICKS, CUMULATIVE-CURVES), and both elements are
        Numpy arrays.

        """
        return self._get_ccs(self.get_car_link_in_cc, links)

    def get_car_out_ccs(self, links=None):
        """Get the outgoing car cumulative curves for registered links.

        Required arguments *links* should be an iterable of link IDs and
        specify the desired links for which the cumulative curves will be
        retrieved. It could also be None, in which case all registered links
        will be used. For backward compatibility, if *links* is not iterable,
        it will be treated as a list of one element. However, that is not
        recommended.

        Return a tuple of (TICKS, CUMULATIVE-CURVES), and both elements are
        Numpy arrays.

        """
        return self._get_ccs(self.get_car_link_out_cc, links)

    def get_truck_in_ccs(self, links=None):
        """Get the incoming truck cumulative curves for registered links.

        Required arguments *links* should be an iterable of link IDs and
        specify the desired links for which the cumulative curves will be
        retrieved. It could also be None, in which case all registered links
        will be used. For backward compatibility, if *links* is not iterable,
        it will be treated as a list of one element. However, that is not
        recommended.

        Return a tuple of (TICKS, CUMULATIVE-CURVES), and both elements are
        Numpy arrays.

        """
        return self._get_ccs(self.get_truck_link_in_cc, links)

    def get_truck_out_ccs(self, links=None):
        """Get the outgoing truck cumulative curves for registered links.

        Required arguments *links* should be an iterable of link IDs and
        specify the desired links for which the cumulative curves will be
        retrieved. It could also be None, in which case all registered links
        will be used. For backward compatibility, if *links* is not iterable,
        it will be treated as a list of one element. However, that is not
        recommended.

        Return a tuple of (TICKS, CUMULATIVE-CURVES), and both elements are
        Numpy arrays.

        """
        return self._get_ccs(self.get_truck_link_out_cc, links)
